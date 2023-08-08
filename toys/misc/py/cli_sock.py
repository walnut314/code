import socket

HOST = 'localhost'
PORT = 9876
ADDR = (HOST,PORT)
BUFSIZE = 4096
videofile = "notes.txt"
client = 0

import fileinput

while True:
    for line in fileinput.input():
        print(line)
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect(ADDR)
        client.send(line)

client.close()


