# https://pythonnumericalmethods.berkeley.edu/notebooks/chapter24.02-Discrete-Fourier-Transform.html

import matplotlib.pyplot as plt
import numpy as np


def DFT(x):
    N = len(x)
    n = np.arange(N)
    k = n.reshape((N,1))
    e = np.exp(-2j * np.pi * k * n / N)
    X = np.dot(e, x)
    return X

plt.style.use('seaborn-poster')

# sample rate
sr = 100
# sampling interval
ts = 1.0/sr
t = np.arange(0,1,ts)

freq = 1.
x = 3*np.sin(2*np.pi*freq*t)

freq = 4
x += np.sin(2*np.pi*freq*t)

freq = 7
x += 0.5*np.sin(2*np.pi*freq*t)

plt.figure(figsize = (8,6))
plt.plot(t, x, 'r')
plt.ylabel('Amplitude')

plt.show()


X = DFT(x)


N = len(X)
n = np.arange(N)
T = N/sr
freq = n/T

plt.figure(figsize=(8,6))
plt.stem(freq, abs(X), 'b', markerfmt=" ", basefmt="-b")
plt.xlabel('Freq (Hz)')
plt.ylabel('DFT Amplitude |X(freq)|')
plt.show()


