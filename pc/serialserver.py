#!/usr/bin/env python3

import socket
import os
import sys
import comms
from threading import Thread
from serial import Serial

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

    def _manage_client(self, client):
        client.settimeout(0)
        self.clients.append(client)
        while self.running:
            try:
                header = client.recv(5)
            except (socket.timeout, BlockingIOError):
                continue
            if len(header) == 0 or header[0] == 0:
                break
            size = comms.to_integer(header[1:5])
            data = client.recv(size)
            while self.lock:
                pass
            self.lock = True
            print('0x{:02X} > serial: {} bytes {!r}'.format(header[0], size, data))
            self.serial.write(header + data)
            self.lock = False
        self.clients.remove(client)
        client.close()

    def _relay_mbed(self):
        while self.running:
            header = self.serial.read(size=5)
            if not header:
                continue
            size = comms.to_integer(header[1:5])
            if size:
                data = self.serial.read(size=size)
            else:
                data = b''
            print('serial > 0x{:02X}: {} bytes {!r}'.format(header[0], size, data))
            for client in self.clients:
                client.send(header + data)

    def run(self):
        self.serial = Serial(self.serial_path, 9600, timeout=1)
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.bind(self.socket_path)
        self.socket.listen(255) # Allow an insane number of clients
        self.running = True

        recv_thread = Thread(target=self._relay_mbed)
        self.threads.append(recv_thread)
        recv_thread.start()

        while self.running:
            client, _ = self.socket.accept()
            thread = Thread(target=self._manage_client, args=(client,))
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

try:
    with Server(comms.SOCKET_PATH, '/dev/ttyACM0') as server:
        server.run()
except OSError:
    print('{0} already exists. Is the server already running? If not, then '
          'delete {0}'.format(comms.SOCKET_PATH))
    sys.exit(1)
except KeyboardInterrupt:
    sys.exit(130)
