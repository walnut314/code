#!/usr/bin/env python3

# molecule files
# https://www.ch.ic.ac.uk/rzepa/mim/environmental/html

import sys

Lookup = {
    'H' :  (1,  6, 0.2),
    'N' :  (1, 17, 0.5),
    'C' :  (1,  3, 0.5),
    'O' :  (1, 19, 0.5)
}

def translateName(inputName):
    return inputName[:-4] + '.vu3'

def readPdb(inputName):
    input = open(inputName, "r")
    result = []
    for line in input:
        if line[:4] == 'ATOM':
            fields = line.split()
            atom = [fields[2]] + fields[4:7]
            result.append(atom)
    input.close()
    return result

def writeVu3(outputName, atoms):
    output = open(outputName, "w")
    for (symbol, X, Y, Z) in atoms:
        if symbol not in Lookup:
            print('Unknown atom "%s"' % symbol, end="", file=sys.stderr)
            sys.exit(1)
        shape, color, radius = Lookup[symbol]
        print(shape, color, radius, X, Y, Z, end="\n", file=output)
    output.close()



for inputName in sys.argv[1:]:
    atoms = readPdb(inputName)
    for a in atoms:
        print(a)
    outputName = translateName(inputName)
    print(outputName)
    writeVu3(outputName, atoms)

