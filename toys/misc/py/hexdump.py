#!/usr/bin/python

import os, sys
import binascii

"""
def hexdump(buf, hexSize):

    PrintHeading()

    # print hex and ascii
    for i in range(0, hexSize, 16):
        for j in range(0,16):
            if (i == 0):
                print "%08x " % i,
            else:
                byteValue = buf[i+j]
#                print "%02x " % binascii.hexlify(byteValue),
                print "%s " % binascii.hexlify(byteValue),
        print " ",
        for j in range(0,16):
            byteValue = buf[i+j]
            if (byteValue >= 0x20 and byteValue <= 0x7f):
                print "%c" % byteValue,
            else:
                print '.',
        print
    return
"""

def PrintHeading():
    print("Offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E OF ASCII")
    print("------------------------------------------------------------")
    return

def hexdump2(src, length=16):
    FILTER = ''.join([(len(repr(chr(x))) == 3) and chr(x) or '.' for x in range(256)])
    lines = []
    for c in xrange(0, len(src), length):
        chars = src[c:c+length]
        hex = ' '.join(["%02x" % ord(x) for x in chars])
        printable = ''.join(["%s" % ((ord(x) <= 127 and FILTER[ord(x)]) or '.') for x in chars])
        lines.append("%04x  %-*s  %s\n" % (c, length*3, hex, printable))
    return ''.join(lines)

"""
# hexdump3("BUF:", 0, buffer)
def hexdump3(prefix, address, data, offset=0):
    if prefix is not None: print prefix
    if type(data) is str: data = map(ord, data)
    length = len(data)
    while length > 0:
        size = min(length, 8)
        line = '%08X:  ' % (address) + ' '.join(map(lambda x: "%02X" % x, data[offset:offset+size]))
        address += size
        mark = offset
        offset += size
        length -= size
        if length > 0:
            size = min(length, 8)
            line += '  ' + ' '.join(map(lambda x: "%02X" % x, data[offset:offset+size]))
            address += size
            offset += size
            length -= size
        line += ' ' * (61 - len(line)) + '"'
        for c in data[mark:offset]:
            line += (c < 32 or c > 126) and '?' or chr(c)
        line += ' ' * (78 - len(line)) + '"'
        print(line)
        #print line + ' ' * (78 - len(line)) + '"'
"""

def main():
    ret="#012abc"
    #ret="#~½|"
    for c in ret:
        print "%s  -  %02x" % (c,ord(c))

    return

    s = sys.stdin.read()    
    hexdump2(s, 256)
    #hexdump3("BUF:", 0, s)

if __name__=='__main__':
    main()
