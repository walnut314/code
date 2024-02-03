#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

N = input('Enter num points: ')
freq = input('Enter frequency of sine wave: ')
phase = 0 #input('Enter phase of sine wave: ')

tau = 1.0 # time increment
N = int(N) # convert str to int
freq = float(freq)
phase = float(phase)
T = 1 / freq

sampleFrequency = N
sampleInterval = 1 / N
beginTime = 0
endTime = 10

#t = np.linspace(0, N, N)
# 1 second interval in N points
#t = np.linspace(0, 1-1/N, N)
t = np.arange(beginTime, endTime, sampleInterval);
#print(t)
#print(len(t))

y = np.sin(2*np.pi*freq*t + phase)
#print(y)
#print(len(y))

# frequency bins
# f = [0, 1.(N*tau), 2/(N*tau), ...]
f = np.arange(N)/(N*tau) # f = [0, 1.(N*tau), 2/(N*tau), ...]
#print(f)

### compute the transform
yt = np.zeros(N, dtype=complex)

import time
startTime = time.time()
yt = np.fft.fft(y)
stopTime = time.time()
#print(len(yt));


print('Elapsed time = ', stopTime - startTime, ' seconds')

freqs = np.fft.fftfreq(len(yt))
#for i in range(len(yt)):
#    for ys in yt:
#        idx = np.argmax(np.abs(yt))
#        freq = freqs[idx]
#        if yt[i].imag > 10.0:
#            print(i, yt[i].imag, f[i])


print("min, max")
print(freqs.min(), freqs.max())
# Find the peak in the coefficients
idx = np.argmax(np.abs(yt))
freq = freqs[idx]
frate = freq
freq_in_hertz = abs(freq * frate)
print("freq at max")
print(freq_in_hertz)

    
#"""
#Graph the time series and its transform
plt.subplot(1, 2, 1) # Left plot
ax = plt.gca()
#ax.set_xticks(np.arange(0, N, 4))
#ax.set_yticks(np.arange(0, N, 4))
plt.grid()
plt.plot(t, y)
plt.title('Original time series')
plt.xlabel('Time')

plt.subplot(1, 2, 2) # Right plot
ax = plt.gca()
ax.set_xticks(np.arange(0, 1.0, 0.15))
#ax.set_yticks(np.arange(0, N, 4))
plt.plot(f, np.real(yt), '-', f, np.imag(yt), '--')
plt.legend(['Real', 'Imaginery '])
plt.grid()
plt.title('Fourier tranform')
plt.xlabel('Frequency')

plt.show()
#"""

"""
# Compute PSD
powspec = np.empty(N)
for i in range(N):
    powspec[i] = abs(yt[i])**2
plt.semilogy(f, powspec, '-')
plt.title('Power spectrum (unnormalized)')
plt.grid()
plt.xlabel('Frequency')
plt.ylabel('Power')

plt.show()
"""


