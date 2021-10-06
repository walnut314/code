#import select
import serial
#import sys
#import time

comport = 'COM13'
ser = serial.Serial(comport, 115200, timeout=0.1)

ser.write("\x00")
ser.write("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")

ser.write("\x0F")
ser.flushInput()


ser.write("\x00")
ser.write("\x02")
data = ser.read(4)
print "mode {}".format(data)

ser.close()
