#!/usr/bin/env python3

# https://stackoverflow.com/questions/34232664/
#    projectile-motion-simple-simulation-using-numpy-matplotlib-python

import numpy as np
import matplotlib.pylab as plot
import math as m
#initialize variables
#velocity, gravity
v = 30
g = 9.8
#increment theta 25 to 60 then find  t, x, y
#define x and y as arrays

theta = np.arange(m.pi/6, m.pi/3, m.pi/36)

t = np.linspace(0, 5, num=100) # Set time as 'continous' parameter.

for i in theta: # Calculate trajectory for every angle
    x1 = []
    y1 = []
    for k in t:
        x = ((v*k)*np.cos(i)) # get positions at every point in time
        y = ((v*k)*np.sin(i))-((0.5*g)*(k**2))
        x1.append(x)
        y1.append(y)
    p = [i for i, j in enumerate(y1) if j < 0] # Don't fall through the floor
    for i in sorted(p, reverse = True):
        del x1[i]
        del y1[i]

    plot.plot(x1, y1) # Plot for every angle

plot.show() # And show on one graphic

