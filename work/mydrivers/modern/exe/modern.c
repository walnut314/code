#include <windows.h>
#include <stdio.h>

#define DEVICE_NAME       "\\\\.\\Modern"
//
// Device type           -- in the "User Defined" range."
//
#define FILEIO_TYPE 40001
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
#define IOCTL_MODERN_METHOD_IN_DIRECT \
    CTL_CODE( FILEIO_TYPE, 0x900, METHOD_IN_DIRECT, FILE_ANY_ACCESS  )

#define IOCTL_MODERN_METHOD_OUT_DIRECT \
    CTL_CODE( FILEIO_TYPE, 0x901, METHOD_OUT_DIRECT , FILE_ANY_ACCESS  )

#define IOCTL_MODERN_METHOD_BUFFERED \
    CTL_CODE( FILEIO_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS  )

#define IOCTL_MODERN_METHOD_NEITHER \
    CTL_CODE( FILEIO_TYPE, 0x903, METHOD_NEITHER , FILE_ANY_ACCESS  )


void doIo(HANDLE hDevice)
{
    char OutputBuffer[100];
    char InputBuffer[200];
    BOOL bRc;
    ULONG bytesReturned;

    memset(OutputBuffer, 0, sizeof(OutputBuffer));

    bRc = DeviceIoControl ( hDevice,
                            (DWORD) IOCTL_MODERN_METHOD_BUFFERED,
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

int main()
{
    HANDLE hDevice;
    DWORD  errnum = 0;

    hDevice = CreateFile(DEVICE_NAME,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        errnum = GetLastError();
        printf("failed to open device %s err %x\n", DEVICE_NAME, errnum);
    } else {
        printf("success opening %s\n", DEVICE_NAME);
        doIo(hDevice);
        CloseHandle ( hDevice );
    }
}

