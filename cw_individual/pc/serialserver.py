#!/usr/bin/env python3

import socket
import os
import sys
from threading import Thread
from serial import Serial

MAX_PACKET_SIZE = (2 ** 32) - 1
SOCKET_PATH = '/tmp/mbed_serial.socket'


def to_bytes(n, width):
    """Convert integer `n` into `width` bytes discarding excess, big endian."""
    return bytes((n >> ((width - (i + 1)) * 8)) & 0xff for i in range(width))


def to_integer(octets):
    """Convert bytes into an integer, big endian"""
    n = 0
    for i, o in enumerate(octets):
        n |= o << ((len(octets) - (i + 1)) * 8)
    return n


class SerialError(Exception): pass
class ServerNotFoundError(SerialError): pass
class ServerLostError(SerialError): pass


class Client:
    def __init__(self, command):
        self.command = command
        self.socket = None

    def __enter__(self):
        self.connect(SOCKET_PATH)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        try:
            self.disconnect()
        except BrokenPipeError as e:
            raise ServerLostError('Connection to server was lost') from e

    def connect(self, socket_path):
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.settimeout(None)
        try:
            self.socket.connect(socket_path)
        except FileNotFoundError as e:
            msg = 'Server does not appear to be running'
            raise ServerNotFoundError(msg) from e

    def disconnect(self):
        if self.socket:
            self.socket.send(bytes(5))
            self.socket.close()

    def write(self, packet):
        size = len(packet)
        if size > MAX_PACKET_SIZE:
            raise ValueError('packet exceeds maximum packet size')

        header = bytes([self.command]) + to_bytes(size, 4)
        self.socket.send(header + packet)

    def read(self):
        while True:
            header = self.socket.recv(5)
            command = header[0]
            size = to_integer(header[1:5])
            data = self.socket.recv(size)

            if command == self.command:
                return data


class Server:
    def __init__(self, socket_path, serial_path):
        self.clients = []
        self.threads = []
        self.socket_path = socket_path
        self.socket = None
        self.serial_path = serial_path
        self.serial = None
        self.lock = False

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close(not isinstance(exc_value, OSError))

    def _relay_client(self, client):
        client.settimeout(0)
        self.clients.append(client)
        while self.running:
            try:
                header = client.recv(5)
            except (socket.timeout, BlockingIOError):
                continue
            if len(header) == 0 or header[0] == 0:
                break
            size = to_integer(header[1:5])
            data = client.recv(size)
            while self.lock:
                pass
            self.lock = True
            print('0x{:02X} > serial - {} bytes'.format(header[0], size))
            self.serial.write(header + data)
            self.lock = False
        self.clients.remove(client)
        client.close()

    def _relay_serial(self):
        while self.running:
            header = self.serial.read(size=5)
            if not header:
                continue
            size = to_integer(header[1:5])
            if size:
                data = self.serial.read(size=size)
            else:
                data = b''
            print('serial > 0x{:02X} - {} bytes'.format(header[0], size))
            for client in self.clients:
                client.send(header + data)

    def run(self):
        self.serial = Serial(self.serial_path, 9600, timeout=1)
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.bind(self.socket_path)
        self.socket.listen(255)  # Allow an insane number of clients
        self.running = True

        print('Serving {} at {}'.format(self.serial_path, self.socket_path))

        recv_thread = Thread(target=self._relay_serial)
        self.threads.append(recv_thread)
        recv_thread.start()

        while self.running:
            client, _ = self.socket.accept()
            thread = Thread(target=self._relay_client, args=(client,))
            self.threads.append(thread)
            thread.start()

    def close(self, delete_socket_file=True):
        self.running = False
        for thread in self.threads:
            thread.join()

        self.serial.close()
        self.socket.close()
        if delete_socket_file:
            os.remove(self.socket_path)


if __name__ == '__main__':
    try:
        with Server(SOCKET_PATH, '/dev/ttyACM0') as server:
            server.run()
    except OSError:
        print('{0} already exists. Is the server already running? If not, '
              'then delete {0}'.format(SOCKET_PATH))
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(130)
