#!/usr/bin/python

# http://stackoverflow.com/questions/19165259/python-numpy-scipy-curve-fitting
# note: input data needs to sorted by x

import Tkinter as tk
import numpy as np
import matplotlib.pyplot as plt
import csv # https://docs.python.org/2/library/csv.html

SKIP_HEADER       = 1
POLYNOMIAL_DEGREE = 3
#CSV_FILENAME      = "Airspeed_Calibration_From_FLT5.csv" # frame,mcp_spd,ias
CSV_FILENAME      = "new_cal.csv" # frame,mcp_spd,ias
MCP_FIX_FILENAME  = "MCPC_Std_All.csv"

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

#for x in flt_data:
#    print x
#print flt_data
#exit()

x = air_array[1:,0] # start on 1st element not zero-th
y = air_array[1:,1] 

# calculate polynomial
z = np.polyfit(x, y, POLYNOMIAL_DEGREE) # 
f = np.poly1d(z)

#x_new = np.linspace(x[0], x[-1], 150)
x_new = np.linspace(x[0], x[-1])
y_new = f(x_new)

plt.plot(x,y,'o', x_new, y_new)
plt.xlim([x[0]-1, x[-1] + 1 ])
plt.show()


for mcp_speed in range(20, 140, 10):
    adp_ias_cal = float(np.interp([mcp_speed], x, y))
    print "{0}, {1}".format(mcp_speed, adp_ias_cal)

exit()

### now rip through the ADP CSV and 
mcp_fixfile = open(MCP_FIX_FILENAME, 'rb')
mcp_fix_rows = csv.reader(mcp_fixfile, delimiter=',', quotechar='|')

if (SKIP_HEADER):
    next(mcp_fix_rows) # skip first line header
for line in mcp_fix_rows:       
    frame = int(line[0].strip())
    mcp_speed = float(line[21])
    adp_ias_cal = float(np.interp([mcp_speed], x, y))
    print "{0}, {1}".format(frame, adp_ias_cal)

exit()

