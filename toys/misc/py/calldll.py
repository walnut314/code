#!/usr/bin/python3
from ctypes import *

libc = cdll.LoadLibrary("MSVCRT.DLL")
printf = libc.printf
printf(b"hello world\n")

import ctypes

#user32 = ctypes.WinDLL('user32', use_last_error=True)
libc2 = ctypes.WinDLL('msvcrt', use_last_error=True)
libc2.printf(b"dude wusup\n")

