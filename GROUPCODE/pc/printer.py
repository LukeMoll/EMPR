#!/usr/bin/env python3

import sys
from datetime import datetime

import serialserver

try:
    with serialserver.Client(0x01) as serial:
        print('Connected to', serialserver.SOCKET_PATH)
        newline = True
        while True:
            msg = serial.read().decode('ascii')
            if newline:
                time = datetime.now().strftime('%F %T')
                print('[{}] {}'.format(time, msg), end='', flush=True)
            else:
                print(msg, end='', flush=True)
            newline = msg[-1] == '\n'
except serialserver.SerialError as e:
    print(e)
