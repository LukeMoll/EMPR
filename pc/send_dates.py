#!/usr/bin/env python3

import sys
from datetime import datetime

import comms

try:
    with comms.Client(0x02) as serial:
        print("Serial open")
        while True:
            serial.read()
            print("Request received")

            now = datetime.now()
            year = now.year
            month = now.month
            day = now.day
            hour = now.hour
            minute = now.minute
            second = now.second

            packet = bytes([month, day, hour, minute, second])
            serial.write(packet)
            print("Datetime sent")
except comms.SerialError as e:
    print(e)
