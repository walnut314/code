import serial
import time

# http://dangerousprototypes.com/docs/SPI

port = "COM13"

def outb(register, channel, value):
    pass

def inb(register, channel):
    pass

def ser_init():
    global ser
    ser = serial.Serial(port, 115200, timeout=1)
    assert ser.isOpen()
        
def send(ser,cmd):
    """send the command and listen to the response."""
    #print "send {}".format(cmd)
    ser.write(str(cmd+'\n').encode('ascii')) # send our command
    for line in ser.readlines(): # while there's a response
        print(line.decode('utf-8').strip()) # show it


def response(byte_count=1, return_data=False):
    data = self.port.read(byte_count)
    if byte_count == 1 and return_data == False:
        if data == chr(0x01): return 1
        else: return 0
    else:
        return data


#	def bulk_trans(self, byte_count=1, byte_string=None):
#		if byte_string == None: pass
#		self.port.write(chr(0x10 | (byte_count-1)))
#		#self.timeout(0.1)
#		for i in range(byte_count):
#			self.port.write(chr(byte_string[i]))
#			#self.timeout(0.1)
#		data = self.response(byte_count+1, True)
#		return data[1:]

#        self.send_start_bit();
#        stat = self.bulk_trans(2, [i2caddr<<1, cmd]);
#        self.send_stop_bit();
#        return stat[0] == chr(0x01)


def detect(addr,val):
    byte_count = 2
    ser.write("\x02") # start bit
    ser.write("\x03") # stop bit
    ser.write(chr(0x10 | (byte_count-1)))
    ser.write(addr)
    ser.write(val)
    #x = ser.read(byte_count+1)
    x = response(byte_count+1)
    print "resp: {}".format(x)
    return x


def enum():
    for addr in range(0,128):
        if detect(addr, 0x5A) == True:
            print "NACK addr {:02X}".format(addr)
        else:
            print "detected device addr {:02X}".format(addr)

def spi_init():
    send(ser,'#') # reset bus pirate (slow, maybe not needed)
    send(ser,"m5") # change mode (goal is to get away from HiZ)
    send(ser,'\n') # mode 5 is spi
    send(ser,'\n') # mode 5 is spi
    send(ser,'\n') # mode 5 is spi
    send(ser,'\n') # mode 5 is spi
    send(ser,'\n') # mode 5 is spi
    send(ser,'\n') # mode 5 is spi
    send(ser,'W') # turn power supply to ON.
    send(ser,'P') # pullups
    #send(ser,"(1)")

def i2c_init():
    send(ser,'#') # reset bus pirate (slow, maybe not needed)
    send(ser,'m') # change mode (goal is to get away from HiZ)
    send(ser,'4') # mode 4 is i2c
    send(ser,'W') # turn power supply to ON.
    send(ser,'P') # pullups
    send(ser,"(1)")
    
def spi_read():
    send(ser,"[r]")

ser_init()
#i2c_init()
spi_init()
#enum()
while (True):
    spi_read()
   

