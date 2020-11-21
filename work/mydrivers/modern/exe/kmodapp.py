#!/usr/bin/env python3

import sys
import struct
import win32file
import win32api
import win32con
import winioctlcon


MODERN_DEVICE = "Modern"

def main():
    device_name = "\\\\.\\{:s}:".format(MODERN_DEVICE)
    print("device: ", device_name)
    access_flags = win32con.GENERIC_WRITE | win32con.GENERIC_READ
    share_flags = 0
    creation_flags = win32con.OPEN_EXISTING
    attributes_flags = 0
    hDevice = win32file.CreateFile(device_name, access_flags, share_flags, None, creation_flags, attributes_flags, None)

    if hDevice != None:
        print("sweet")
    else:
        print("oh horrors")

#    buf_len = struct.calcsize(vol_data_buf_fmt)
#    for i in [buf_len]:
#        print("    Passing a buffer size of: {:d}".format(i))
#        buf = win32file.DeviceIoControl(hDevice, winioctlcon.FSCTL_GET_NTFS_VOLUME_DATA, None, i)
#        print("    DeviceIocontrol returned a {:d} bytes long {:}".format(len(buf), type(buf)))
#        out = struct.unpack_from(vol_data_buf_fmt, buf)
#        print("\n    NumberSectors: {:}\n    TotalClusters: {:}\n    BytesPerCluster: {:}".format(out[1], out[2], out[6]))
    win32api.CloseHandle(hDevice)

if __name__ == "__main__":
    print("Python {:s} on {:s}\n".format(sys.version, sys.platform))
    main()
