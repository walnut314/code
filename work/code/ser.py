#!/usr/bin/env python3

import time
import serial

PORT = 'com4' #customize this! Find it in device manager.
BAUD = 3000000
ser=serial.Serial(PORT, BAUD, timeout=1) # is com free?
if ser.isOpen():
    print("open")
    print(ser.name)
#assert ser.isOpen() #throw an exception if we aren't connected
#ser.write(commands.get('RESET'))
while True:
    line = ser.readline()
    if line:
        print(line.hex())

ser.close() # disconnect so we can access it from another app

