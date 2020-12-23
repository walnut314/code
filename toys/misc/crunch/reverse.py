#!/usr/bin/env python3

import sys

input = open(sys.argv[1], "r")
lines = input.readlines()
input.close()

lines.reverse()
output = open(sys.argv[2], "w")
for line in lines:
#   print >> output, line.strip()
    print(line, end="", file=output)
output.close()

