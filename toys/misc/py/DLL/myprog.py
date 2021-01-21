#!/usr/bin/python3
from ctypes import *

libc = cdll.LoadLibrary("mydll.dll")
hello = libc.hello
hello()

"""
import ctypes

#user32 = ctypes.WinDLL('user32', use_last_error=True)
libc2 = ctypes.WinDLL('msvcrt', use_last_error=True)
libc2.printf(b"dude wusup\n")
"""
