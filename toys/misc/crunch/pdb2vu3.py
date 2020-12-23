#!/usr/bin/env python3

import sys

def readPdb(inputName):
    input = open(inputName, "r")
    result = []
    for line in input:
        fields = line.split()
        print(fields)
    input.close()
    return result

for inputName in sys.argv[1:]:
    atoms = readPdb(inputName)
    for a in atoms:
        print(a)
#   outputName = translateName(inputName)
#   writeVu3(outputName, atoms)

