#define _GNU_SOURCE
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_NAME       "\\\\.\\PhysicalDrive0"

void hexdump(const unsigned char *buffer, unsigned long long length)
{
    char OffsetBuf[10];
    unsigned long long LastStart = 0, i, k, j;
    unsigned long long elf_bias = -62;
    for (i = 0 ; i < length ; i++) {
        if (i % 16 == 0) {
            sprintf(OffsetBuf, "%08llx", i);
            printf("%4s:", OffsetBuf);
        }
        if (i % 4 == 0) {
            printf(" ");
        }
        printf("%02x ", buffer[i]);
        if (i % 16 == 15 || i == length-1) {
            if (i == length-1) {
                for (k = i % 16 ; k < 15 ; k++) {
                     printf( "   " );
                     if (k % 4 == 3 && k % 16 != 0) {
                         printf(" ");
                     }
                }
            }
            printf("|");
            for (j = LastStart ; j <= i ; j++) {
                if (j % 4 == 0) {
                    printf(" ");
                }
                if (buffer[j] > 31 && buffer[j] < 127) {
                    printf("%c", buffer[j]);
                } else {
                    printf(".");
                }
            }
            if (i == length-1) {
                printf("\n");
                break;
            }
            printf("\n");
            LastStart += 16;
        }        
    }
}

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
    DWORD      dwValueNotUsed;
    OVERLAPPED ovFilePosition      = {0};
    DRIVE_LAYOUT_INFORMATION dli;
    DISK_GEOMETRY geo;
    DWORD dwBufSize, dwBytesReturned;
    BYTE     BootSect[512];
    BYTE     PartSect[1024*32];
    BOOL       fResult;
    size_t len = 0;
    ssize_t read;
    char line[256];
    char *pline = line;

    hDevice = CreateFile(DEVICE_NAME,
                            GENERIC_READ | GENERIC_WRITE, 
                            FILE_SHARE_READ | FILE_SHARE_WRITE, 
                            NULL, 
                            OPEN_EXISTING, 
                            0, // no attribs
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
            if (strncmp("quit", line, 4) == 0) {
                printf("quit\n");
                break;
            } else 
            if (strncmp("help", line, 4) == 0) {
                printf("start, stop, send\n");
            } else
            if (strncmp("boot", line, 4) == 0) {
                  fResult = ReadFile(hDevice, 
                                     BootSect,
                                     512,
                                     &dwValueNotUsed,
                                     &ovFilePosition);
                  if (TRUE == fResult) {
                      hexdump(BootSect, 512);
                  }

            } else
            if (strncmp("part", line, 4) == 0) {

                dwBufSize = sizeof(DRIVE_LAYOUT_INFORMATION) +
                                sizeof(PARTITION_INFORMATION)*3;

                fResult = DeviceIoControl(hDevice, 
                                          IOCTL_DISK_GET_DRIVE_GEOMETRY, //IOCTL_DISK_GET_DRIVE_LAYOUT,
                                          NULL, 0, // input buffer & size
                                          &geo, dwBufSize, // output buffer & size
                                          &dwBytesReturned, NULL);

                if (TRUE == fResult) {
                    printf("got the geometry\n");
                    printf("Cylinders: %ld\n", geo.Cylinders);
                    printf("media type: %x\n", geo.MediaType);
                    printf("TracksPerCyl: %d\n", geo.TracksPerCylinder);
                    printf("SectorsPerTrack: %d\n", geo.SectorsPerTrack);
                    printf("BytesPerSector: %d\n", geo.BytesPerSector);
                } else {
                    errnum = GetLastError();
                    if (errnum == ERROR_INSUFFICIENT_BUFFER) {
                        printf("buffer too small %d\n", dwBytesReturned);
                    } else {
                        printf("unknown error %d\n", errnum);
                    }
                }
            }

            printf("$ ");
        }
        CloseHandle (hDevice);
    }
}

