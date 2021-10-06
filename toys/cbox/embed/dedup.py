#!/usr/bin/python

# http://stackoverflow.com/questions/19165259/python-numpy-scipy-curve-fitting
# note: input data needs to sorted by x

import Tkinter as tk
import numpy as np
import matplotlib.pyplot as plt
import csv # https://docs.python.org/2/library/csv.html

SKIP_HEADER       = 1
POLYNOMIAL_DEGREE = 3
CSV_FILENAME      = "Airspeed_Calibration_From_FLT5.csv" # frame,mcp_spd,ias

air_array = np.array([[0,0]]) # this causes zeroth value to be set to zero - so skip it
csvfile = open(CSV_FILENAME, 'rb')
csv_rows = csv.reader(csvfile, delimiter=',', quotechar='|')
speed_data_set = set()
if (SKIP_HEADER):
    next(csv_rows) # skip first line header
for line in csv_rows:       
    frame = int(line[0].strip())
    mcp_speed = float(line[1])
    adp_ias = float(line[2])
    i_mcp_speed = int(mcp_speed)
    if i_mcp_speed not in speed_data_set:
        speed_data_set.add(i_mcp_speed)
        i_adp_ias = int(adp_ias)
        air_array = np.append(air_array, [[i_mcp_speed,i_adp_ias]], axis=0)
        print "{0}, {1}, {2}".format(frame, mcp_speed, adp_ias)

exit()


