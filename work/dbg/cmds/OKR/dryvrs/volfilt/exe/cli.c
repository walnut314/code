#include <stdio.h> 
#include <stdlib.h> 
#include <stddef.h>
#include <windows.h>  
#include <devioctl.h>  
#include "genkfilt_ioctl.h"

extern HANDLE connect_start_event();

int __cdecl main(int argc, char **argv)
/*++

Routine Description:

    This is the main function. It takes no arguments from the user.

Arguments:

    None

Return Value:

  Status

--*/
{
    //enumhdc();
    HANDLE hDevice = INVALID_HANDLE_VALUE, hEvent = INVALID_HANDLE_VALUE;
    DWORD ret = 0, size;
    BOOL reset, status;
    char deviceName[256];
    int bytesret = 0, err, i;
    PQUERY_SETTINGS query = NULL;
    GENK_DEV set = {0};
    int cmd = 0;

    if (*argv[1] == 'q') {
        printf("cli query\n");
        cmd = 'q';
    } else
    if (*argv[1] == 's') {
        printf("cli set\n");
        cmd = 's';
    } else
    if (*argv[1] == 'r') {
        printf("cli reset\n");
        cmd = 'r';
    } else
    if (*argv[1] == 'c') {
        printf("cli clear\n");
        cmd = 'c';
    } else
    if (*argv[1] == 'v') {
        printf("cli listen for event\n");
        cmd = 'v';
    }
    
    memset(deviceName, 0, 256);
    if (cmd == 'v') {
        connect_start_event();
    } else {
        sprintf(deviceName, "\\\\.\\filter");

        hDevice = CreateFile (deviceName,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, //0,
                              NULL, // no SECURITY_ATTRIBUTES structure
                              OPEN_EXISTING, // No special create flags
                              0, // No special attributes
                              NULL);
        if (hDevice == INVALID_HANDLE_VALUE) {
            printf("error opening device %s %d\n", deviceName, GetLastError());
            return 0;
        }
    }

    if (cmd == 'q') {
        printf("success opening %s\n", deviceName);
        size = sizeof(QUERY_SETTINGS);
retry_query_ioctl:
        query = (PQUERY_SETTINGS) malloc(size);
        memset(query, 0, size);
        if (query) {
            status = DeviceIoControl( hDevice, 
                                  IOCTL_GENKFILT_QUERY_SETTINGS, 
                                  query, size, 
                                  query, size, 
                                  &bytesret, NULL ); 
    
            if (!status) {
                err = GetLastError();
                printf("error query ioctl %d\n", GetLastError());
                if (err = ERROR_INSUFFICIENT_BUFFER) {
                    USHORT data = *((PUSHORT) query);
                    printf("insufficient buffer, bytes ret %d, data %d\n", bytesret, data);
                    free(query);
                    size *= 4;
                    goto retry_query_ioctl;
                }
            } else {
                printf("success ioctl\n");
                printf(" no disks: %d\n", query->NumItems);
                for (i = 0; i < query->NumItems; i++) {
                    printf("Device Type: %x\n", query->dev[i].DeviceType);
                    printf("Serial No:   %d\n", query->dev[i].SerialNo);
                    printf("Disk No.:    %d\n", query->dev[i].DiskNumber);
                    printf("Blocked:     %d\n\n", query->dev[i].Blocked);
                }
            }
        }
        CloseHandle(hDevice);
        return 0;
    }
    if (cmd == 's') {
        set.DiskNumber = 1;
        set.Blocked = TRUE;
        status = DeviceIoControl( hDevice, 
                                  IOCTL_GENKFILT_SET, 
                                  &set, sizeof(set), 
                                  NULL, 0, 
                                  &bytesret, NULL ); 
        
        if (!status) {
            err = GetLastError();
            printf("error set ioctl %d\n", GetLastError());
        } else {
            printf("success set\n");
        }
        CloseHandle(hDevice);
        return 0;
    }
    if (cmd == 'r') {
        status = DeviceIoControl( hDevice, 
                                  IOCTL_GENKFILT_RESET, 
                                  NULL, 0, 
                                  NULL, 0, 
                                  &bytesret, NULL ); 
        
        if (!status) {
            err = GetLastError();
            printf("error reset ioctl %d\n", GetLastError());
        } else {
            printf("success reset\n");
        }
        CloseHandle(hDevice);
        return 0;
    }
    if (cmd == 'c') {
        set.DiskNumber = 1;
        set.Blocked = FALSE;
        status = DeviceIoControl( hDevice, 
                                  IOCTL_GENKFILT_SET, 
                                  &set, sizeof(set), 
                                  NULL, 0, 
                                  &bytesret, NULL ); 
        
        if (!status) {
            err = GetLastError();
            printf("error clr ioctl %d\n", GetLastError());
        } else {
            printf("success clear\n");
        }
        CloseHandle(hDevice);
        return 0;
    }
}


