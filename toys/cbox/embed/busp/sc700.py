import time
from pyBusPirateLite.I2Chigh import *


# http://www.nxp.com/documents/application_note/AN10397.pdf

# Table 3. ASCII commands supported by SC18IM700
# ASCII command Hex value Command function
# S 0x53 I2C-bus START
# P 0x50 I2C-bus STOP
# R 0x52 read SC18IM700 internal register
# W 0x57 write to SC18IM700 internal register
# I 0x49 read GPIO port
# O 0x4F write to GPIO port
# Z 0x5A power down

SC700_BRG0        = 0x00  # RW    0xF0
SC700_BRG1        = 0x01  # RW    0x02
SC700_PortConf1   = 0x02  # RW    0x55
SC700_PortConf2   = 0x03  # RW    0x55
SC700_IOState     = 0x04  # RW    0x0F
SC700_reserved    = 0x05  # --    0x00
SC700_I2CAdr      = 0x06  # RW    0x26
SC700_I2CClkL     = 0x07  # RW    0x13
SC700_I2CClkH     = 0x08  # RW    0x13
SC700_I2CO        = 0x09  # RW    0x66
SC700_I2CStat     = 0x0A  # R     0xF0


def outb(register, channel, value):
    reg = register
    print "write: {:02X}, {:02X}".format(reg, value)
    i2c.set_byte(i2c_addr, reg, value)
    
def inb(register, channel):
    reg = register
    i2c.set_byte(i2c_addr, reg, 0x52)
    data = i2c.get_byte(i2c_addr, reg)
    print "read: {:02X} => {:02X}".format(reg, data)
    return data

def i2c_init():
    global i2c
    global i2c_addr
    global port

    i2c_addr = 0x13
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

#print i2c.get_byte(0x48, 0x00)
#print i2c.set_byte(0x48, 0x0E, 0x08)

def uart_init():
    pass
    # configure the UART
    
def dump_regs():
    for i in range(0,11):
        #print "reg {:02x}h => {:02x}h".format(i, inb(i,0))
        print "reg {:02x}h => {:02x}h".format(i, i2c.get_byte(0x48, i))
        
def dump(a, len):
    for i in range(0,len):
        print "[{}] : {:02x}h".format(i, i2c.get_byte(0x48, i))
    
def enum():
    for addr in range(0,128):
        if i2c.detect(addr, 0x5A) == True:
            print "NACK addr {:02X}".format(addr)
        else:
            print "detected device addr {:02X}".format(addr)
        #start = i2c.send_start_bit()
        #stop = i2c.send_stop_bit()
        #print "start: {}, stop {}".format(start, stop)
        #if stop == 0:
        #    print "found device at {:02X}".format(addr)
        #else:
        #    print "no device at {:02X} start{}.stop{}".format(addr, start, stop)

def main():
    i2c_init()
    enum()
    dump_regs()
    #a = [0,0,0,0,0,0,0,0]
    #while True:
    #    #x = raw_input('>> ')
    #    #if (x == 'q'):
    #    #    break
    #    #xx = int(x)
    #    xx = 0x42
    #    #print "write: {}".format(xx)
    #    channel = UART_750_TX_FIFO
    #    outb(UART_SCRATCH, channel, xx);
    #    channel = UART_750_RX_FIFO
    #    v = inb(UART_SCRATCH, channel)
    #    #v = inb(UART_SCRATCH)
    #    #print "write: {:02X}".format(xx)
    #    #print "read: {:02X}".format(v)
    #    #i2c.detect(i2c_addr, xx)
    #    time.sleep(1)

if __name__ == '__main__':
    main()

