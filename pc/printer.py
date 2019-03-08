#!/usr/bin/env python3

import sys
from datetime import datetime

import comms

try:
    with comms.Client(0x01) as serial:
        while True:
            time = datetime.now().strftime('%F %T')
            msg = serial.read().decode('ascii')
            print('[{}] {}'.format(time, msg), end='', flush=True)
except comms.SerialError as e:
    print(e)
