#!/usr/bin/env python3

import tkinter as tk
import _thread
import time
import queue
import random
import threading
import serial


class Application(tk.Frame):
    def __init__(self, endCommand, serial, queue, master=None):
        self.endCommand = endCommand
        tk.Frame.__init__(self, master)
        self.ser = serial
        self.playing = False
        self.size = 100
        self.playText()
        self.grid()
        self.createWidgets()
        self.queue = queue
        self.processIncoming()
        #open a serial port
    
    def createWidgets(self):
        top = self.winfo_toplevel()
        top.rowconfigure(0, weight=1)
        top.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.quitButton = tk.Button(self, text = 'quit', command = self.endCommand)
        self.quitButton.grid(row = 0, column = 1, sticky = tk.N+tk.S+tk.E+tk.W)
        self.playPause = tk.Button(self, text = self.playText(), command = self.play)
        self.playPause.grid(row = 0, column = 0, sticky = tk.N+tk.S+tk.E+tk.W)
        self.canvas = tk.Canvas(width=200, height=200)
        self.canvas.grid(row = 1, column =0, sticky = tk.N+tk.S+tk.E+tk.W)
        
    
    def play(self):
        self.playing = not self.playing
        #write something over serial, that if they read, they toggle play/pause
        self.playPause = tk.Button(self, text = self.playText(), command = self.play)
        self.playPause.grid(row = 0, column = 0, sticky = tk.N+tk.S+tk.E+tk.W)
        self.ser.write(1)
        if self.playing:
            self.canvas.create_oval(50, 50, (50+self.size), (50+self.size), fill='blue') 
        else:
            self.canvas.delete("all")
            #eventually, add a command that will send to serial that causes it to pause

    def playText(self):
        if self.playing:
            return 'pause'
        else: return 'play '
    
    def processIncoming(self):
        while self.queue.qsize():
            try:
                self.size = self.queue.get(0)
            except queue.Empty:
                pass

class ThreadedClient(Application):
    def __init__(self, master, serial):
        self.master = master
        self.queue = queue.Queue()
        self.ser = serial

        self.gui = Application(self.endApplication, self.ser, self.queue, None) #change parametres around so it works

        self.running = 1
        self.thread1 = threading.Thread(target=self.workerThread)
        self.thread1.start

        self.periodicCall()

    def periodicCall(self):
        self.gui.processIncoming()
        if not self.running:
            import sys
            sys.exit(1)
        self.master.after(200, self.periodicCall)
    
    def workerThread(self):
        while self.running:
            #read the serial
            self.master.after(100)
            msg = self.ser.read(1)
            self.queue.put(msg)
    
    def endApplication(self):
        self.running = 0
    

rand = random.random
app = tk.Tk()
ser = serial.Serial('dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)   
client = ThreadedClient(app, ser)
app.mainloop()





