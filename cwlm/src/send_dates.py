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

print(second)

print(month)

ser = serial.Serial('/dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE) # opens serial port
num = str(month)
if len(num) < 2:
	num = "0" + num
ser.write(month)
# num = str(day)
# if len(num) < 2:
# 	num = "0" + num
# ser.write(day)
# num = str(hour)
# if len(num) < 2:
# 	num = "0" + num
# ser.write(hour)
# num = str(minute)
# if len(num) < 2:
# 	num = "0" + num
# ser.write(minute)
# num = str(second)
# if len(num) < 2:
# 	num = "0" + num
# ser.write(second)

