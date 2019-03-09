from threading import Thread
import serial
import queue
import tkinter as tk
import serialserver

length = 0 #length of thing, in seconds (you can calculate that)

class Gui(Thread, TimeCalculator, serialserver, tk.Frame):
    
    def __init__(self, master = None):
        tk.Frame(self, master)
        Thread.__init__(self)
        queue.__init__(self)
        GuiSetup(self)
        GuiRun(self)
        self.queue = queue
        self.colour = 'fff'
        self.size = 50
        self.alpha = TimeCalculator.alpha
        self.serial = serialserver.Client(0x06)

    def GuiSetup(self):
        top = self.winfo_toplevel()
        top.rowconfigure(0, weight = 1)
        top.columnconfigure(0, weight = 1)
        self.rowconfigure(0, weight = 1)
        self.columnconfigure(0, weight = 1)
        self.canvas = tk.Canvas(width=200, height=200)
        self.canvas.create_oval(50, 50, (50+self.size), (50+self.size), fill=self.colour)
        self.canvas.attributes("-alpha", self/alpha)
        self.update()
        self.update()
        #setup the initial gui (without widgets, they just mess things up)
    
    def GuiRun(self):
        def inter():
            while self.alpha > 0:
                try:
                    self.colour = self.serial.read() #checks if there's anything in queue
                    self.size += 10
                except:
                    pass
            self.update_idletasks()
            self.update()
                #update gui values (size, colour, alpha)
        def time(): 
            TimeCalculator.CalcRun
        
        t1 = inter()
        t2 = time()
        t1.start()
        t2.start()


class TimeCalculator(Thread, start_length):

    def __init__(self):
        Thread.__init__()
        self.delta = round((length/20), 2)
        self.alpha = 1
        CalcRun(self)
    
    def CalcRun(self):
        while self.alpha > 0:
            sleep(delta)
            self.alpha -= 0.05



coms = SerialCommunication()
timecalc = TimeCalculator()
gui = Gui()

coms.start()
gui.start(coms.queue)
timecalc.start()




    
