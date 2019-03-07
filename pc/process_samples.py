import numpy as np
from scipy.io import wavfile

def removeDC(buf):
    mean = np.mean(buf)
    print("Mean:", mean)
    return buf - mean

if __name__ == "__main__":
    import sys
    with open(sys.argv[1]) as f:
        next(f) # discard first element
        a = np.fromiter(map(lambda l: int(l.strip()),f), np.int16)
        b = removeDC(a)
        wavfile.write("processed.wav",  8000, b)