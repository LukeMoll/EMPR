import wave
import numpy as np
from math import trunc, ceil, floor
from scipy.io import wavfile

def main(fn):
    ar = np.absolute(load(fn))
    numbuckets = 160

    buckets = average_(ar, numbuckets)
    for l in makeBlockStrings(buckets[0], numlines=3):
        print("[",l,"]")

    # bad one
    buckets = average(ar, numbuckets)
    for l in makeBlockStrings(buckets, numlines=3):
        print("[",l,"]")

def load(fn):
    """
        fn: filename
        returns: numpy array [2,N] for left/right and N samples
    """
    f = wavfile.read(fn) # whole file
    _a = np.array(f[1], dtype=float)#[:framerate * 30]
    ar = np.transpose(_a)
    return ar

def average(ar, numbuckets): # delet plz
    ar = np.absolute(ar)
    bucketlen = int(ceil(ar.shape[1]/numbuckets))
    buckets = []
    for i in range(numbuckets):
        mean = np.mean(ar[1,i*bucketlen : (i+1)*bucketlen])
        buckets.append(mean)
    return buckets

def average_(ar, numbuckets):
    B = floor(ar[1].size / numbuckets)
    rpad = B - (ar[1].size % B)
    ar = np.pad(ar, ((0,0),(0,rpad)), 'constant')
    ar = np.swapaxes(ar.reshape(2,-1,B),1,2)
    means = np.mean(ar,axis=1)
    return means

def getType(sampwidth):
    if   sampwidth <=  8: return np.int8
    elif sampwidth <= 16: return np.uint16
    elif sampwidth <= 32: return np.uint32
    else:                 return np.uint64

blocks = [" ", "▁", "▂", "▃", "▄", "▅", "▆","▇","█"]

def makeBlockStrings(means, maxval=None, numlines=2):
    if type(means) != list and len(means.shape) == 2: means = means[1]
    res = numlines * 8
    if maxval is None: maxval = max(means)
    scaled = list(map(lambda m: int(abs(round((m*res)/maxval))), means))
    lines = []
    # we could list comprehension this but this works fine
    for i in range(numlines):
        lines.append("")
        for v in scaled:
            ind = min(max(v-i*8,0),8)
            lines[i]+=(str(blocks[ind]))
    lines.reverse()
    return lines



if __name__ == "__main__":
    main("testaudio/Infantile.wav")