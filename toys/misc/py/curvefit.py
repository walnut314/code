#!/usr/bin/env python3

import Tkinter as tk
import numpy as np
import matplotlib.pyplot as plt

# http://stackoverflow.com/questions/19165259/python-numpy-scipy-curve-fitting

#points = np.array([(1, 1), (2, 4), (3, 1), (9, 3)])

# stock prices on consecutive days -- see buy sell pattern
points = np.array([(0, 100), (1, 180), (2, 260), (3, 310), (4, 40), (5, 535), (6, 695)])
# get x and y vectors
x = points[:,0]
y = points[:,1]

# calculate polynomial
z = np.polyfit(x, y, 3)
f = np.poly1d(z)

# calculate new x's and y's
x_new = np.linspace(x[0], x[-1], 50)
y_new = f(x_new)

plt.plot(x,y,'o', x_new, y_new)
plt.xlim([x[0]-1, x[-1] + 1 ])
plt.show()
