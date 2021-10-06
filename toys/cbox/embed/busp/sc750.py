import sys
import time
from pyBusPirateLite.I2Chigh import *

# http://www.nxp.com/documents/application_note/AN10462.pdf
# https://courses.engr.illinois.edu/ece390/resources/serial.html
# https://github.com/juhasch/pyBusPirateLite/blob/master/tests/test_bitbang.py

# SC700 definitions
SC700_START       = 0x53 # S 0x53 I2C-bus START
SC700_STOP        = 0x50 # P 0x50 I2C-bus STOP
SC700_READ        = 0x52 # R 0x52 read SC18IM700 internal register
SC700_WRITE       = 0x57 # W 0x57 write to SC18IM700 internal register
SC700_RDGPIO      = 0x49 # I 0x49 read GPIO port
SC700_WRGPIO      = 0x4F # O 0x4F write to GPIO port
SC700_PWRDN       = 0x5A # Z 0x5A power down

# SC700 register addresses
# register          addr    RW    reset
#=========          ====    ==    =====
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

# Polling constants
POLLIN            = 0x0001
POLLOUT           = 0x0004
POLLRDNORM        = 0x0040
POLLWRNORM        = 0x0100

# SC750 FIFO channel definitions
TX_CHANNEL        = 0x00
RX_CHANNEL        = 0x01

UART_RX_FIFO      = 0x00  # R
UART_TX_FIFO      = 0x00  # W
UART_DLATCH_LO    = 0x00  # RW EX
UART_DLATCH_HI    = 0x01  # RW EX
UART_INTR_EN      = 0x01  # RW
UART_INTR_ID      = 0x02  # R
UART_FIFO_CTRL    = 0x02  # W
UART_LINE_CTRL    = 0x03  # RW
UART_MODEM_CTRL   = 0x04  # RW
UART_LINE_STAT    = 0x05  # R
UART_MODEM_STAT   = 0x06  # R
#UART_SCRATCH      = 0x07  # RW


# SC750 register addresses
# register          addr    description
#=========          ====    ===============================
SC750_RHR         = 0x00    #Receive Holding Register (RHR) 
SC750_THR         = 0x00    #Transmit Holding Register (THR)
SC750_IER         = 0x01    #Interrupt Enable Register (IER) Interrupt Enable Register
SC750_IIR         = 0x02    #Interrupt Identification Register (IIR) 
SC750_FCR         = 0x02    #FIFO Control Register (FCR)
SC750_LCR         = 0x03    #Line Control Register (LCR) Line Control Register
SC750_MCR         = 0x04    #Modem Control Register (MCR)[1] Modem Control Register[1]
SC750_LSR         = 0x05    #Line Status Register (LSR) n/a
SC750_MSR         = 0x06    #Modem Status Register (MSR) n/a
SC750_SPR         = 0x07    #Scratchpad Register (SPR) Scratchpad Register
SC750_TCR         = 0x06    #Transmission Control Register (TCR)[2] Transmission Control Register[2]
SC750_TLR         = 0x07    #Trigger Level Register (TLR)[2] Trigger Level Register[2]
SC750_TXLVL       = 0x08    #Transmit FIFO Level Register n/a
SC750_RXLVL       = 0x09    #Receive FIFO Level Register n/a
SC750_IODir       = 0x0A    #I/O pin Direction Register I/O pin Direction Register
SC750_IOState     = 0x0B    #I/O pin States Register n/a
SC750_IOIntEna    = 0x0C    #I/O Interrupt Enable Register I/O Interrupt Enable Register
SC750_reserved2   = 0x0D
SC750_IOControl   = 0x0E    #I/O pins Control Register I/O pins Control Register
SC750_EFCR        = 0x0F    #Extra Features Register Extra Features Register
SC750_DLL         = 0x00    #divisor latch LSB (DLL)[4] divisor latch LSB[4]
SC750_DLH         = 0x01    #divisor latch MSB (DLH)[4] divisor latch MSB[4]
SC750_EFR         = 0x02    #Enhanced Feature Register (EFR)[5] Enhanced Feature Register[5]
SC750_XON1        = 0x04    #Xon1 word[5] Xon1 word[5]
SC750_XON2        = 0x05    #Xon2 word[5] Xon2 word[5]
SC750_XOFF1       = 0x06    #Xoff1 word[5] Xoff1 word[5]
SC750_XOFF2       = 0x07    #Xoff2 word[5] Xoff2 word[5]


def outw(register, channel, word):
    reg = (register << 3 | channel) & 0xFF
    print "WRITEW: {:02X}, {:02X}".format(reg, word)
    i2c.set_word(i2c_addr, reg, word)

def outb(register, channel, value):
    reg = (register << 3 | channel) & 0xFF
    print "WRITE: 0x{:02X} : 0x{:02X}".format(reg, value)
    i2c.set_byte(i2c_addr, reg, value)
    
def inb(register, channel):
    reg = (register << 3 | channel) & 0xFF
    data = i2c.get_byte(i2c_addr, reg)
    print "READB: 0x{:02X} : 0x{:02X}".format(reg, data)
    return data

def inw(register, channel):
    reg = (register << 3 | channel) & 0xFF
    data = i2c.get_word(i2c_addr, reg)
    print "READW: 0x{:02X} : 0x{:04X}".format(reg, data)
    return data

def outbx(addr, value, register):
    i2c.set_byte(addr, register, value)
    
def inbx(addr, register):
    return i2c.get_byte(addr, register)

def i2c_init():
    global i2c
    global i2c_addr
    global port
    i2c_addr = 0x48
    port = "COM13"
    bitrate = 1
    timeout = 5
    i2c = I2Chigh(port, bitrate, timeout)
    i2c.resetBP()
    i2c.BBmode()    # bit bang mode
    i2c.enter_I2C() # i2c mode
    # turn on the power supply and enable the pullups
    i2c.cfg_pins(I2CPins.POWER | I2CPins.PULLUPS)
    print i2c

def uart_init():
    # configure the UART
    outb(UART_INTR_EN,   0x00);
    outb(UART_FIFO_CTRL, 0xC7);
    outb(UART_LINE_CTRL, 0x83);
    outb(UART_DLATCH_LO, 0x01);
    outb(UART_DLATCH_HI, 0x00);
    outb(UART_LINE_CTRL, 0x03);
    outb(UART_MODEM_CTRL, 0x03);
    inb( UART_MODEM_STAT );
    inb( UART_LINE_STAT );
    inb( UART_RX_FIFO );
    inb( UART_INTR_ID );
#   outb(, UART_INTR_EN, INTR_MASK);
    outb(UART_MODEM_CTRL, 0x00);
    print " Interrupt-ID: {:02X}".format(inb( UART_INTR_ID ) )
    
def uart_read(a, n_bytes):
    line_status = inb( UART_LINE_STAT );
    if ( (line_status & 1) == 0 ):
        while (inb( UART_LINE_STAT ) & 1):
            # need timeout here
            print "waiting for read"
    count = 0;
    for i in range(0,n_bytes):
        a[i] = inb( UART_RX_FIFO )
        count = count + 1
        if ( (inb( UART_LINE_STAT ) & 1) == 0 ):
            break
    return  count

def uart_write(a, n_bytes):
    modem_status = inb( UART_MODEM_STAT )
    if ( (modem_status & 0x10 ) != 0x10 ):
        if ((inb( UART_MODEM_STAT ) & 0x10) == 0x10 ):
            return -EINTR
    count = 0;
    for datum in range(0,n_bytes):
        while ( (inb( UART_LINE_STAT ) & 0x20) == 0 ):
            pass
        outb(UART_TX_FIFO, datum)
        count = count + 1
        if ( (inb( UART_MODEM_STAT ) & 0x10) != 0x10 ):
            break
    return  count

def uart_poll():
    mask = 0;
    if ( inb( UART_LINE_STAT ) & 0x01 ):
        mask |= (POLLIN | POLLRDNORM)
    if ( inb( UART_LINE_STAT ) & 0x20 ):
        mask |= (POLLOUT | POLLWRNORM)
    print "mask: {}".format(mask)
    return  mask

def uart_exit():
    channel = 0
    outb(UART_INTR_EN, channel, 0x00)
    outb(UART_MODEM_CTRL, channel, 0x00)

# define a function to report the seconds:
def get_seconds():
    return i2c.get_byte(i2c_addr, 0x00)

def dump_regs():
    for i in range(0,16):
        #print "reg {:02x}h => {:02x}h".format(i, inb(i,0))
        print "reg {:02x}h => {:02x}h".format(i, i2c.get_byte(0x48, i))
        
def dump(a, len):
    for i in range(0,len):
        print "[{}] : {:02x}h".format(i, i2c.get_byte(0x48, i))
    
def scan():
    print "Scanning for i2c devices:"
    devices = []
    for addr in range(0,128):
        if i2c.detect(addr, 0x5A) == True:
            sys.stdout.write('.')
        else:
            devices.append(addr)
    print ""
    if (len(devices) > 0):
        print "found {} device(s): ".format(len(devices))
        for d in devices:
            print "  i2c addr: {:02X}h".format(d)
    else:
        print "no devices found"
    print ""

def scratch_pad_ping(value):
    channel = TX_CHANNEL
    outb(SC750_SPR, channel, value);
    channel = RX_CHANNEL
    v = inb(SC750_SPR, channel)
    return v
    
def sc700_fetch_register(reg):
    #outb(SC750_THR, TX_CHANNEL, SC700_START);
    outb(SC750_THR, TX_CHANNEL, SC700_READ)
    outb(SC750_THR, TX_CHANNEL, reg)
    outb(SC750_THR, TX_CHANNEL, SC700_STOP)
    #rx = inb(SC750_LSR, RX_CHANNEL)
    #print "LSR {:02X}".format(rx)
    data = inb(SC750_RHR, RX_CHANNEL)
    return data

BMP085_CONTROL           = 0xF4 
BMP085_TEMPDATA          = 0xF6
BMP085_PRESSUREDATA      = 0xF6
BMP085_READTEMPCMD       = 0x2E
BMP085_READPRESSURECMD   = 0x34
def sc700_slave_read(slave_addr, num_bytes):
    outb(SC750_THR, TX_CHANNEL, SC700_START)
    outb(SC750_THR, TX_CHANNEL, (slave_addr<<1)) # write
    outb(SC750_THR, TX_CHANNEL, 2) # num bytes to write
    outb(SC750_THR, TX_CHANNEL, BMP085_CONTROL)
    outb(SC750_THR, TX_CHANNEL, BMP085_READTEMPCMD)
    outb(SC750_THR, TX_CHANNEL, SC700_STOP)
    outb(SC750_THR, TX_CHANNEL, SC700_START)
    outb(SC750_THR, TX_CHANNEL, (slave_addr<<1)|1) # read
    outb(SC750_THR, TX_CHANNEL, 1) # num bytes to read
    outb(SC750_THR, TX_CHANNEL, SC700_STOP)
    data = inb(SC750_RHR, RX_CHANNEL)
    return data
    
def sc750_init():
    outb(SC750_IOControl, TX_CHANNEL, 0x80) # reset sc750
    outb(SC750_FCR, TX_CHANNEL, 0x07) # enable fifo (C7)
    outb(SC750_LCR, TX_CHANNEL, 0x83) # enable special registers, and line control (8:N:1)
    # divisor = 72d = 48h = 11.0592 MHz / (9600 baud * 16) see section 7.8
    outb(SC750_DLH, TX_CHANNEL, 0x00) # baud rate divisor (high byte)
    outb(SC750_DLL, TX_CHANNEL, 0x48) # baud rate divisor (low byte)
    outb(SC750_LCR, TX_CHANNEL, 0x03) # disable special registers
    outb(SC750_MCR, TX_CHANNEL, 0x00) # modem control - set RTS/DTR high
    x = inb(SC750_MSR, RX_CHANNEL)
    print "MSR {:02X}".format(x)
    x = inb(SC750_LSR, RX_CHANNEL)
    print "LSR {:02X}".format(x)
    x = inb(SC750_RHR, RX_CHANNEL)
    print "RHR {:02X}".format(x)
    x = inb(SC750_IIR, RX_CHANNEL)
    print "IIR {:02X}".format(x)
    outb(SC750_MCR, TX_CHANNEL, 0x00) # modem control - set RTS/DTR low

def main():
    i2c_init()
    
    dump_regs()
    sc750_init()
    scan()

    #uart_poll()
    #a = [0,0,0,0,0,0,0,0]
    #uart_read(a, 8)
    #uart_write(a, 8)
    #dump(a, 8)

    sp = 0x42
    v = scratch_pad_ping(sp)
    if (v == sp):
        print "passed scratch register test"
    else:
        print "failed scratch register test"

    while True:
        #scratch_pad_ping(sp)
        #outb(SC750_THR, TX_CHANNEL, 0x01)
        #outb(SC750_THR, TX_CHANNEL, 0xFF)
        #for reg in range (0,11):
        #    x = sc700_fetch_register(reg)
        #    print "700: reg {:02X} = {:02X}".format(reg, x)
        slave_addr = 0x77
        y = sc700_slave_read(slave_addr, 1)
        print "BMP temp: {}".format(y)
        print ""
        time.sleep(1)

if __name__ == '__main__':
    main()

