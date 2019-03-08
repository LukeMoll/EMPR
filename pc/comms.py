import socket

MAX_PACKET_SIZE = (2 ** 32) -1
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
            raise ServerNotFoundError('Server does not appear to be running') from e

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
