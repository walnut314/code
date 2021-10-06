#"""python3 control of buspirate (SWHarden.com)"""
# http://www.swharden.com/wp/2016-07-14-controlling-bus-pirate-with-python/
# http://dangerousprototypes.com/docs/1-Wire

# M:\code\buspy>ser.py
# send #
# #
# RESET
# 
# Bus Pirate v3b
# Firmware v5.10 (r559)  Bootloader v4.4
# DEVID:0x0447 REVID:0x3046 (24FJ64GA002 B8)
# http://dangerousprototypes.com
# HiZ>
# send m
# m
# 1. HiZ
# 2. 1-WIRE
# 3. UART
# 4. I2C
# 5. SPI
# 6. 2WIRE
# 7. 3WIRE
# 8. LCD
# 9. DIO
# x. exit(without change)


import time
import serial

commands = {
        'BBIO1': '\x00',    # Enter reset binary mode
        'SPI1':  '\x01',    # Enter binary SPI mode
        'I2C1':  '\x02',    # Enter binary I2C mode
        'ART1':  '\x03',    # Enter binary UART mode
        '1W01':  '\x04',    # Enter binary 1-Wire mode
        'RAW1':  '\x05',    # Enter binary raw-wire mode
        'RESET': '\x0F',    # Reset Bus Pirate
        'STEST': '\x10',    # Bus Pirate self-tests
}

BUSPIRATE_PORT = 'com13' #customize this! Find it in device manager.

def send(ser,cmd):
    """send the command and listen to the response."""
    #print "send {}".format(cmd)
    ser.write(str(cmd+'\n').encode('ascii')) # send our command
    for line in ser.readlines(): # while there's a response
        print(line.decode('utf-8').strip()) # show it

ser=serial.Serial(BUSPIRATE_PORT, 115200, timeout=1) # is com free?
assert ser.isOpen() #throw an exception if we aren't connected
send(ser,'#') # reset bus pirate (slow, maybe not needed)
send(ser,'m') # change mode (goal is to get away from HiZ)
send(ser,'4') # mode 9 is i2c
#send(ser,'0') # mode 9 is DIO
#send(ser,'w') # turn power supply to OFF. Uppercase W for ON, Lowercase w for OFF.
send(ser,'W') # turn power supply to ON. Uppercase W for ON, Lowercase w for OFF.
send(ser,'P') # pullups
send(ser,"(1)"
#send(ser,'6') # mode 9 is DIO
#send(ser,'v') # show current voltages
#for i in range (0,4):
#    mode = chr(ord('0') + i)
#    send(ser, 'm')
#    send(ser, mode)
#    time.sleep(1)

#ser.write(commands.get('RESET'))
while True:
    cmd = raw_input()
    print "cmd: {}".format(cmd)
    send(ser,'?') # pullups

#    got = ser.readline()
#    if not got:
#        break
#    print(got),
#    pass

print "hello"
ser.close() # disconnect so we can access it from another app
#print("disconnected!") # let the user know we're done.
