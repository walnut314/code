#!/usr/bin/env python3

import re
import sys
import colorama
from colorama import Fore, Back

regexp = "(import)"

color = Back.BLACK + Fore.BLUE
print(color + "\nhere we go")
try:
    for line in sys.stdin:
        if line:
            line = line.rstrip()
            cap = re.search(regexp, line)
            if (cap):
                n = len(cap.groups())
                color = Fore.YELLOW + Back.BLACK
                print(color + "-> " + str(n))
    raise Exception
except Exception:
    color = Back.BLACK + Fore.RED
    print(color, "now you're fucked")
finally:
    print(Fore.RESET)

