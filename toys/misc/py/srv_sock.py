#!/usr/bin/env python3

import socket

HOST = 'localhost'
PORT = 9876
ADDR = (HOST,PORT)
BUFSIZE = 4096

serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

serv.bind(ADDR)
serv.listen(5)

print('listening ...')

while True:
    conn, addr = serv.accept()
    #print('client connected ... ', addr)

    while True:
        data = conn.recv(BUFSIZE)
        if not data: break
        print(str(data.decode('utf-8')))
        if "quit" in str(data):
            exit()

    conn.close()
    #print('client disconnected')
