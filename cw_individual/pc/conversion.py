#!/usr/bin/env python

import ffmpy
import time
import scipy.io.wavfile

toWav = input('Please enter the name of the file you wish to convert (including the extension): ') #Obtains the name of the file for conversion.
rawWav = ''

for i in range(0, (len(toWav) - 1)): #Removes the file type for naming, changes to wav and adds the date.
        if (toWav[i] != '.'):
                rawWav = rawWav + toWav[i]
        else:
                rawWav = rawWav + str(time.strftime('%Y-%m-%d')) + '.wav'
                break

print(toWav) #For debugging purposes; shows the name of the file it is converting and the name it will have after conversion.
print(rawWav)

Conversion = ffmpy.FFmpeg( #Converts the chosen file to a wav file.
                executable='/home/userfs/j/jlp548/Downloads/ffmpeg-git-20190302-amd64-static/ffmpeg', #The file path on my computer for ffmpeg (using the PATH variable wasn't working so I had to force it).
                inputs = {toWav: None},
                outputs = {rawWav: None}
                )

Conversion.run() #Runs the conversion.

sample_Rate, data = scipy.io.wavfile.read(rawWav)
print(sample_Rate)
print(data.shape)
python_Data = [[data[y][x] for x in range(data.shape[1])] for y in range(data.shape[0])] #Turns out numpy integers can't be too large and were overflowing to negative, so copied it to this python 2d list to remove overflow.
i = data.shape[0] #The size of the array is obtained to run through and manipulate the data
j = data.shape[1]
offset = 0 #Going to be used to offest all the integers by a set amount, to make all integers positive.

for x in range(0,i): #Runs through all the data in the array and obtains the smallest value for the offset.
    for y in range(0,j):
        if python_Data[x][y] < offset:
            offset = python_Data[x][y]

#biggest = 0 #For debugging

if offset != 0: #If a negative number was found, do...
    for x in range(0,i): #Runs through the the data and offsets it all
        for y in range(0,j):
            python_Data[x][y] = '{0:018b}'.format(python_Data[x][y] + (offset * -1)) #Formats the integers to binary
            #if python_Data[x][y] > biggest: #For debugging
            #    biggest = python_Data[x][y]
            #if python_Data < 0:
            #    print(python_Data)

#print(python_Data) #For debugging
#print(biggest) #For debugging

f = open((rawWav[:-4] + ".txt"), "w")
for x in range(0,i): #Runs through all the data in the array and outputs it to a text file.
    for y in range(0,j):
	f.write(python_Data[x][y] + "\n")
f.close()

input('Press any key to exit:')
