#!/usr/bin/python

import base64
import sys
import pdb; pdb.set_trace()


def encode(key, clear):
    enc = []
    for i in range(len(clear)):
        key_c = key[i % len(key)]
        enc_c = chr((ord(clear[i]) + ord(key_c)) % 256)
        enc.append(enc_c)
    return base64.urlsafe_b64encode("".join(enc))

def decode(key, enc):
    dec = []
    enc = base64.urlsafe_b64decode(enc)
    for i in range(len(enc)):
        key_c = key[i % len(key)]
        dec_c = chr((256 + ord(enc[i]) - ord(key_c)) % 256)
        dec.append(dec_c)
    return "".join(dec)


str = raw_input("Enter name: ")
#str = sys.stdin
print str
#exit()

key = "peanutbutterandjellysandwich"
str1 = encode(key, str)
print str1
str1 = raw_input("Enter cipher: ") 
str2 = decode(key, str1)
print str2

