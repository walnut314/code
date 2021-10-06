import time
import serial

ser = serial.Serial(
    port='com13',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()

print "Buspirate terminal emulator. version 2.0.4"
print ":{}".format(ser)

input=1
out = ''

while True:
    #input = raw_input(">> ")
    input = raw_input(out)
    if input == 'exit':
        ser.close()
        exit()
    else:
        # send the character to the device
        # (note a \r\n carriage return and line feed required by my device)
        ser.write(input + '\r\n')
        out = ''
        # let's wait one second before reading output (let's give device time to answer)
        time.sleep(1)
        while ser.inWaiting() > 0:
            out += ser.read(1)

        #if out != '':
        #    print out
        #    #print ">>" + out
