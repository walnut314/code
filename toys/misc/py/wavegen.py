#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as np
import csv
import sys

Fs = 1024
f = 512 # frequency
sample = Fs
x = np.arange(sample)
y = np.sin(2 * np.pi * f * x / Fs)
#print x, y
#print y[0]
#print y[3]
#print y[5]
#exit()

# http://stackoverflow.com/questions/25735153/plotting-a-fast-fourier-transform-in-python
sp = np.fft.fft(y)
freq = np.fft.fftfreq(x.shape[-1])
plt.plot(freq, sp.real, freq, sp.imag)
plt.show()

#exit()

plt.plot(x, y)
plt.xlabel('sample(n)')
plt.ylabel('voltage(V)')

plt.show()

f = open("wave.csv", 'wt')
try:
    writer = csv.writer(f)
    writer.writerow( ('Sample', 'Voltage') )
    for i in range(sample):
        writer.writerow( (i, y[i]) ) 
finally:
    f.close()


