#define _GNU_SOURCE
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "..\sys\public.h"

/* Fake getline function */
size_t getline(char** line, size_t *len, FILE *h)
{
    size_t rlen;
    char *pline = *line;
    fgets(pline, *len-1, h);
    rlen = strlen(pline);
    return rlen-1;
}

void doIoctl(HANDLE hDevice, DWORD ioctl)
{
    char OutputBuffer[100];
    char InputBuffer[200];
    BOOL bRc;
    ULONG bytesReturned;

    memset(OutputBuffer, 0, sizeof(OutputBuffer));

    bRc = DeviceIoControl ( hDevice,
                            ioctl,
                            InputBuffer,
                            (DWORD) strlen( InputBuffer )+1,
                            OutputBuffer,
                            sizeof( OutputBuffer),
                            &bytesReturned,
                            NULL
                            );
    
    if ( !bRc )
    {
        printf ( "Error in DeviceIoControl : %d\n", GetLastError());
        return;
    }
    printf("    OutBuffer (%d): %s\n", bytesReturned, OutputBuffer);
}

int main()
{
    HANDLE hDevice;
    DWORD  errnum = 0;
    size_t len = 0;
    ssize_t read;
    char line[256];
    char *pline = line;

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

        printf("$ ");
        fflush(stdin);
        len = 80;
        while ((read = getline(&pline, &len, stdin)) != -1) {
            //printf("Retrieved line of length %zu : %s\n", read, line);
            if (strncmp("quit", line, 4) == 0) {
                printf("quit\n");
                break;
            } else 
            if (strncmp("help", line, 4) == 0) {
                printf("start, stop, send\n");
            } else
            if (strncmp("start", line, 5) == 0) {
                doIoctl(hDevice, IOCTL_MODERN_START_THREAD);
                printf("start\n");
            } else 
            if (strncmp("stop", line, 4) == 0) {
                doIoctl(hDevice, IOCTL_MODERN_STOP_THREAD);
                printf("stop\n");
            } else
            if (strncmp("send", line, 4) == 0) {
                doIoctl(hDevice, IOCTL_MODERN_QUEUE_REQUEST);
                printf("send\n");
            }
            printf("$ ");
        }
        CloseHandle (hDevice);
    }
}

