#!/usr/bin/python

import Tkinter as tk
import numpy as np
import matplotlib.pyplot as plt
import csv # https://docs.python.org/2/library/csv.html
import re

RPM_LOCATION        = 0
ROTOR_LOCATION      = 1
ENGINE_LOCATION     = 2
ROTOR_SLOPE         = 3
ENGINE_SLOPE        = 4
CAL_WIDTH           = 5
CAL_LENGTH          = 8

#   [RPM,   RotorTime,      EngineTime,      RotorSlope,     EngineSlope]
# Cal Date 6/7/2017 using RPM data logger on R44
cal_array = np.array([
    [56,    25620.01036,    13244.83679,     0,              0          ],
    [60,    24086.24444,    12452.6303,     -0.00260796,    -0.005049189],
    [70,    20375.06415,    10531.35849,    -0.002694561,   -0.005725374],
    [80,    17753.31435,     9175.813212,   -0.003814247,   -0.007377105],
    [90,    15926.65737,     8215.741036,   -0.005474482,   -0.010415884],
    [100,   14380.98168,     7420.903141,   -0.006469662,   -0.012581182],
    [102,   14139.21429,     7300.766917,   -0.008272414,   -0.016647768],
    [110,   13467.46602,     6947.635922,   -0.011909223,   -0.022654483] ]);

# old r44 calibration//Cal Date 6/6/2017
cal_array2 = np.array([
    [54,     27200,             13800,      0,               0          ],
    [70,     21538,             10600,     -0.002826087,    -0.005      ],
    [80,     18000,              9400,     -0.001477273,    -0.008333333],
    [90,     15800,              8400,     -0.004545455,    -0.01       ],
    [100,    14700,              7500,     -0.009090909,    -0.011111111],
    [104,    14400,              7400,     -0.013333333,    -0.04       ],
    [110,    13200,              6800,     -0.005,          -0.01       ] ]);

# Cal Data for R22
cal_array3 = np.array([
    [55,     24000,             21200,     -0.003409091,    -0.00326087 ], 
    [70,     19600,             16600,     -0.003409091,    -0.00326087 ],   
    [80,     17100,             14400,     -0.004,          -0.004545455],
    [90,     15200,             13200,     -0.005263158,    -0.008333333],
    [97,     14100,             12300,     -0.006363636,    -0.007777778],
    [104,    13300,             11400,     -0.00875,        -0.007777778],
    [110,    12500,             10900,     -0.0075,         -0.012      ] ]);

def getInterp(array, pulse_width, pulse_location, slope_location):
    for i in range(0,CAL_LENGTH):
        pulse = array[i,pulse_location]    
        slope = array[i,slope_location]
        rpm   = array[i,RPM_LOCATION]
        if (pulse_width >= pulse): # the array is sorted descending
            delta_t = pulse - pulse_width
            interp = rpm - (slope * delta_t)
            print "{0} rpm: {1} pulse: {2} slope: {3:f} dt: {4} interp: {5:f}".format(i,rpm,pulse,slope,delta_t,interp)
            break;

def my_range(start, end, step):
    while start <= end:
        yield start
        start += step

def main():
    getInterp(cal_array3,20000,ROTOR_LOCATION,ROTOR_SLOPE)
    exit()

    myblob = open(r'2017_06_22_121208_RPMlog.txt', 'r').readlines()
    for line in myblob:
        line = line.rstrip()
        cap = re.match(r'([\d]{2,24})', line, re.I | re.S)
        if (cap):
            frame,rotor,motor,cpd,sync = line.split(",")
            print "frame: {0} rotor: {1} motor: {2} cpd: {3} sync: {4}".format(frame,rotor,motor,cpd,sync)

if __name__ == '__main__':
    main()

