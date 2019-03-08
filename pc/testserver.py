#!/usr/bin/env python3

import socket
import comms

try:
    with comms.Client(0x01) as serial:
        serial.write('hello, world!'.encode('ascii'))
        while True:
            print(serial.read().decode('ascii'))
except FileNotFoundError:
    print('Serial server does not appear to be running.')
