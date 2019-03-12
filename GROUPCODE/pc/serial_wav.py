"""
    for use with src/test_audiorec_serial
"""

from scipy.io import wavfile
import numpy as np
import serial
from datetime import timedelta, datetime
import time
from process_samples import removeDC

try:
    # stolen from send_dates.py for a working serial config
    ser = serial.Serial('/dev/ttyACM0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
except Exception as e:
    print("Could not acquire serial handle: {}".format(e))
    exit()

buf = None
written = None
period = None
length = None
start = None

def progress(current, mx):
    print("[{:.2f}%]  {} out of {}{}                          ".format(
        (current / mx)*100, 
        current, 
        mx, 
        '.' * (current % 3))
    , end="\r")

print("Waiting for start of new transfer")
for line in ser: 
    if not line.startswith(b'BUFFER DUMP'): pass
    else:
        print(line)
        break
for line in ser:
    if line.startswith(b'length:'):
        length = int(line[7:].decode(encoding='ascii').strip())
        buf = np.zeros((length,), dtype=np.int16)
        written = 0
        print("Length:",length,"samples.")
    elif line.startswith(b'periodus:'):
        period = int(line[9:].decode(encoding='ascii').strip())
        print("Period: {}us ({} KHz).".format(period, 1000/period))
    elif line.startswith(b'0x'):
        if buf is None or written is None or period is None:
            print("Sample recieved before all variables set!")
            print("buf:", buf,"\nwritten:", written, "\nperiod:", period)
            exit()
        if written == 0:
            start = time.time()
        if written >= length:
            print("Excess samples recieved!")
        # shusb pylint, `buf` will be a numpy array by now

        buf[written] = int(line.decode(encoding="ascii").strip()[2:], base=16)
        written += 1
        if(written & 31 == 0):
            # print(".", end="", flush=True)
            progress(written, length)
    elif line.startswith(b'BUFFER DUMP END'):
        print("{} samples written out of {}.".format(written, length))
        print("Transfer took {}".format(timedelta(seconds=time.time() - start)))
        break

    elif line.startswith(b'BUFFER DUMP') or True:
        print(line.decode(encoding="ascii").strip())
    

fnbase = datetime.today().isoformat().split(".")[0].replace(":","-").replace("T","_")
wavfile.write("{}_buf.wav".format(fnbase),  1000000 // period, buf[1:])
wavfile.write("{}_nodc.wav".format(fnbase), 1000000 // period, removeDC(buf[1:]))
with open("{}_samples.csv".format(fnbase), 'w+') as s:
    for v in buf:
        s.write(str(v) + "\n")
