#include <windows.h>
#include <stdio.h>

#include "..\sys\public.h"

#if 0
void doIo(HANDLE hDevice)
{
    char OutputBuffer[100];
    char InputBuffer[200];
    BOOL bRc;
    ULONG bytesReturned;

    memset(OutputBuffer, 0, sizeof(OutputBuffer));

    bRc = DeviceIoControl ( hDevice,
                            (DWORD) IOCTL_NONPNP_METHOD_BUFFERED,
                            InputBuffer,
                            (DWORD) strlen( InputBuffer )+1,
                            OutputBuffer,
                            sizeof( OutputBuffer),
                            &bytesReturned,
                            NULL
                            );
    
    if ( !bRc )
    {
        printf ( "Error in DeviceIoControl : %d", GetLastError());
        return;

    }
    printf("    OutBuffer (%d): %s\n", bytesReturned, OutputBuffer);
}
#endif

int main()
{
    HANDLE hDevice;
    DWORD  errnum = 0;

    hDevice = CreateFile(XX_WIN32_DEVICE_NAME,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        errnum = GetLastError();
        printf("failed to open device %s err %x\n", XX_WIN32_DEVICE_NAME, errnum);
    } else {
        printf("success opening %s\n", XX_WIN32_DEVICE_NAME);
        //doIo(hDevice);
        CloseHandle ( hDevice );
    }
}

