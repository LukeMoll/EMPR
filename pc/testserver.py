#!/usr/bin/env python3

import socket
import serialserver

try:
    with serialserver.Client(0x01) as serial:
        serial.write('hello, world!'.encode('ascii'))
        while True:
            print(serial.read().decode('ascii'))
except serialserver.SerialError as e:
    print(e)
