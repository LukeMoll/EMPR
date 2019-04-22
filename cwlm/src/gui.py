#!/usr/bin/env python3

import tkinter as tk
import _thread
import time
import queue
import random
import threading
import serial


class Application(tk.Frame):
    def __init__(self, endCommand, master=None):
        self.endCommand = endCommand
        tk.Frame.__init__(self, master)
        # self.ser = serial
        self.playing = True
        self.size = 140
        self.playText()
        self.grid()
        self.createWidgets()
        # self.queue = queue
        # self.processIncoming()
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
        if self.playing:
            self.canvas.create_oval(20, 20, (20+self.size), (20+self.size), fill="#888") 
            self.canvas.set_alpha(.1)
        else:
            self.canvas.delete("all")
            #eventually, add a command that will send to serial that causes it to pause

    def playText(self):
        if self.playing:
            return 'pause'
        else: return 'play '

rand = random.random
app = tk.Tk()
gui = Application(app)
app.mainloop()





