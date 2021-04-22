#!/usr/bin/env python3

import re
import sys

regexp = "Pal"
cnt = 1
line = sys.stdin.readline()
while line:
    cap = re.search(regexp, line)
    if cap:
        n = len(cap.groups())
        print("Line {}: {} {}".format(cnt, n, line.strip()))
    else:
        print("Line {}: {}".format(cnt, line.strip()))
    line = sys.stdin.readline()
    cnt += 1


