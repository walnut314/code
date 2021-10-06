import serial

port = serial.Serial("COM11", baudrate=115200, timeout=3.0)

while True:
    port.write("\r\nSay something:")
    rcv = port.read(10)
    port.write("\r\nYou sent:" + repr(rcv))
