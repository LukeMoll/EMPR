#!/usr/bin/env python

import serial
import time
import datetime

#change this to make it actually work


month = datetime.date.today().month
day = datetime.date.today().day
hour = datetime.datetime.now().hour
minute = datetime.datetime.now().minute
second = datetime.datetime.now().second

date = [month, day, hour, minute, second]

ser = serial.Serial('/dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE) # opens serial port
ser.write(date)

print("sent!")
