#!/usr/bin/env python3

import sys
import struct
import win32file
import win32api
import win32con
import winioctlcon
import ctypes
from ctypes import *

class SWITCH_STATE(ctypes.Structure):
    _pack_ = 1
    _fields_ = [("State", c_int)]

def CTL_CODE(DeviceType, Function, Method, Access):
    return ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method);

FILEIO_TYPE      = 40001
METHOD_BUFFERED  = 0x00
FILE_ANY_ACCESS  = 0x00
FILE_READ_ACCESS = 0x01

IOCTL_MODERN_START_THREAD  = CTL_CODE(FILEIO_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS);
IOCTL_MODERN_STOP_THREAD   = CTL_CODE(FILEIO_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS);
IOCTL_MODERN_QUEUE_REQUEST = CTL_CODE(FILEIO_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS);

def main():

    device_name = r'\\.\Modern'
    print("device: ", device_name)
    access_flags = win32con.GENERIC_WRITE | win32con.GENERIC_READ
    share_flags = 0
    creation_flags = win32con.OPEN_EXISTING
    attributes_flags = 0
    hDevice = win32file.CreateFile(device_name, access_flags, share_flags, None, creation_flags, attributes_flags, None)

    if hDevice != None:
        print("sweet")
        state = SWITCH_STATE(State = 0x5A)
        buf_len = ctypes.sizeof(state)
        buf = win32file.DeviceIoControl(hDevice, IOCTL_MODERN_QUEUE_REQUEST, state, state)
        print(hex(state.State))
        win32api.CloseHandle(hDevice)
    else:
        print("oh horrors")

if __name__ == "__main__":
    print("Python {:s} on {:s}\n".format(sys.version, sys.platform))
    main()
