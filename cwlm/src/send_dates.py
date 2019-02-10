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




ser = serial.Serial('/dev/ttyACM0') # opens serial port
ser.write(month)
ser.write(day)
ser.write(hour)
ser.write(minute)
ser.write(second)
