import time
from pyBusPirateLite.I2Chigh import *


# BMP 085 Pressure Sensor => addr = 0x77
BMP085_CONTROL           = 0xF4 
BMP085_TEMPDATA          = 0xF6
BMP085_PRESSUREDATA      = 0xF6
BMP085_READTEMPCMD       = 0x2E
BMP085_READPRESSURECMD   = 0x34


def outb(register, value):
    reg = register
    print "write: {:02X}, {:02X}".format(reg, value)
    i2c.set_byte(i2c_addr, reg, value)
    
def inb(register):
    reg = register
    i2c.set_byte(i2c_addr, reg, 0x52)
    data = i2c.get_byte(i2c_addr, reg)
    print "read: {:02X} => {:02X}".format(reg, data)
    return data

def i2c_init():
    global i2c
    global i2c_addr
    global port

    i2c_addr = 0x77
    port = "COM11"
    bitrate = 1
    timeout = 5

    i2c = I2Chigh(port, bitrate, timeout)
    i2c.resetBP()
    i2c.BBmode()    # bit bang mode
    i2c.enter_I2C() # i2c mode
    # turn on the power supply and enable the pullups
    i2c.cfg_pins(I2CPins.POWER | I2CPins.PULLUPS)
    print i2c

def dump_regs():
    for i in range(0,11):
        #print "reg {:02x}h => {:02x}h".format(i, inb(i,0))
        print "reg {:02x}h => {:02x}h".format(i, i2c.get_byte(i2c_addr, i))
        
def dump(a, len):
    for i in range(0,len):
        print "[{}] : {:02x}h".format(i, i2c.get_byte(i2c_addr, i))
    
def scan():
    print "Scanning for i2c devices:"
    for addr in range(0,128):
        if i2c.detect(addr, 0x5A) == True:
            #print "NACK addr {:02X}".format(addr)
            pass
        else:
            print "  => found device at addr {:02X}".format(addr)

def read_temp():
    outb(BMP085_CONTROL, BMP085_READTEMPCMD)
    time.sleep(1)
    #delay(5);
    #raw = read16(BMP085_TEMPDATA);
    raw = inb(BMP085_TEMPDATA);
    return raw

def read_pressure():
    outb(BMP085_CONTROL, BMP085_READPRESSURECMD) # + (oversampling << 6));
    #raw = read16(BMP085_PRESSUREDATA);
    raw = inb(BMP085_PRESSUREDATA);
    #raw <<= 8;
    #raw |= read8(BMP085_PRESSUREDATA+2);
    #raw >>= (8 - oversampling);
    return raw

def main():
    i2c_init()
    scan()
    dump_regs()
    #a = [0,0,0,0,0,0,0,0]
    while True:
        t = read_temp()
        print "temp = {}".format(t)


if __name__ == '__main__':
    main()

