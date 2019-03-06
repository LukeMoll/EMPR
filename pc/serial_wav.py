"""
    for use with src/test_audiorec_serial
"""

from scipy.io import wavfile
import numpy as np
import serial

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
        
        if written >= length:
            print("Excess samples recieved!")
        buf[written] = int(line.decode(encoding="ascii").strip()[2:], base=16)
        written += 1
        print(".", end="", flush=True)
    elif line.startswith(b'BUFFER DUMP END'):
        print("{} samples written out of {}.".format(written, length))
        break

    elif line.startswith(b'BUFFER DUMP') or True:
        print(line.decode(encoding="ascii").strip())
    
wavfile.write("buf.wav", 1000000 // period, buf)