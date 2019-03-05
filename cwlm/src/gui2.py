from threading import Thread
import serial
import queue
import tkinter as tk

alpha = 1 #fade quotient, fades out at regular intervals
msg = 0 #change to be a queue
length = 0 #length of thing, in seconds (you can calculate that)

class Gui(Thread, queue, tk.Frame):
    
    def __init__(self, queue, master = None):
        tk.Frame(self, master)
        Thread.__init__(self)
        queue.__init__(self)
        GuiSetup(self)
        GuiRun(self)
        self.queue = queue
        self.colour = 'fff'
        self.size = 50

    def GuiSetup(self):
        global alpha
        top = self.winfo_toplevel()
        top.rowconfigure(0, weight = 1)
        top.columnconfigure(0, weight = 1)
        self.rowconfigure(0, weight = 1)
        self.columnconfigure(0, weight = 1)
        self.canvas = tk.Canvas(width=200, height=200)
        self.canvas.create_oval(50, 50, (50+self.size), (50+self.size), fill=self.colour)
        self.canvas.attributes("-alpha", alpha)
        self.update()
        self.update()
        #setup the initial gui (without widgets, they just mess things up)
    
    def GuiRun(self):
        global alpha
        while alpha > 0:
            try:
                self.colour = self.queue.get() #checks if there's anything in queue
                self.size += 10
            except Queue.Empty:
                pass
        self.update_idletasks()
        self.update()
            #update gui values (size, colour, alpha)

class SerialCommunication(Thread, serial, queue):

    def __init__ (self):
        Thread.__init__(self)
        self.ser = serial.Serial('/dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
        self.queue = queue.Queue()
        ComsSetup(self)
        ComsRun(self)
    
    def ComsSetup(self):
        global length
        length = ser.read(1)
    
    def ComsRun(self):
        while alpha > 0:
            msg = ser.read(1)
            if msg != 0:
                #convert it to hex and string
                msgstring = "{:03x}".format(msg)
                self.queue.put(msgstring)

class TimeCalculator(Thread):

    def __init__(self):
        Thread.__init__()
        global length
        global alpha
        self.delta = round((length/20), 2)
        CalcRun(self)
    
    def CalcRun(self):
        global alpha
        while alpha > 0:
            sleep(delta)
            alpha -= 0.05


coms = SerialCommunication()
timecalc = TimeCalculator()
gui = Gui()

coms.start()
gui.start(coms.queue)
timecalc.start()




    
