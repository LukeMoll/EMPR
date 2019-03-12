#!/usr/bin/env python3

import serialserver
from gui2 import Gui
import queue


interface = Gui(None)

try:
    with serialserver.Client(0x05) as serial:
        print('Connected to', serialserver.SOCKETPATH)
        time = serial.read()
        interface.GuiSetup()
        interface.GuiRun()
    
