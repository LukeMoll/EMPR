from serial import Serial

MAX_PACKET_SIZE = (2 ** 32) -1


def to_bytes(n, width):
    """Convert integer `n` into `width` bytes discarding excess, big endian."""
    return bytes((n >> ((width - (i + 1)) * 8)) & 0xff for i in range(width))


def to_integer(octets):
    """Convert bytes into an integer, big endian"""
    n = 0
    for i, o in enumerate(octets):
        n |= o << ((len(octets) - (i + 1)) * 8)
    return n


def open_serial():
    """Open a serial connection to the mbed"""
    return Serial('/dev/ttyACM0', 9600, timeout=None)


def wait_for_packet(serial, command):
    """Wait until a relevant packet comes in from the mbed and read it"""
    while True:
        header = serial.read(size=5)
        command_byte = header[0]
        size = to_integer(header[1:5])

        if size:
            data = serial.read(size=size)
        else:
            data = b''

        if command == command_byte:
            return data


def send_packet(serial, command, packet):
    """Send a packet to the mbed"""
    size = len(packet)
    if size > MAX_PACKET_SIZE:
        raise ValueError('packet exceeds maximum packet size')

    header = bytes(command) + to_bytes(size, 4)
    serial.write(header + packet)
