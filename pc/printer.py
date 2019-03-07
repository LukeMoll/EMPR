#!/usr/bin/env python3

import sys
import comms

COMMAND = 0x01

try:
    ser = comms.open_serial()
    while True:
        data = comms.wait_for_packet(ser, COMMAND)
        print(data.decode('ascii'), end='', flush=True)
except KeyboardInterrupt:
    sys.exit(130)
