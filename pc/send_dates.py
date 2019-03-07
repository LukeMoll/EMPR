#!/usr/bin/env python3

import sys
from datetime import datetime

import comms

COMMAND = 0x02

ser = comms.open_serial()
print("Serial open")
comms.wait_for_packet(ser, COMMAND)
print("Request received")

now = datetime.now()
year = now.year
month = now.month
day = now.day
hour = now.hour
minute = now.minute
second = now.second

packet = bytes([month, day, hour, minute, second])
comms.send_packet(ser, COMMAND, packet)
print("Datetime sent")
