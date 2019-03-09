#!/usr/bin/env python3

import sys
from datetime import datetime

import serialserver

try:
    with serialserver.Client(0x01) as serial:
        print('Connected to', serialserver.SOCKET_PATH)
        while True:
            time = datetime.now().strftime('%F %T')
            msg = serial.read().decode('ascii')
            print('[{}] {}'.format(time, msg), end='', flush=True)
except serialserver.SerialError as e:
    print(e)
