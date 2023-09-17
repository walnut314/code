#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <windows.h>


// https://msdn.microsoft.com/en-us/library/ff802693.aspx
//
char PORT[] = "com3"; // #customize this! Find it in device manager.
int BAUD = 38400; //3000000;

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

BOOL WriteABuffer(HANDLE hComm, char * lpBuf, DWORD dwToWrite)
{
    OVERLAPPED osWrite = {0};
    DWORD dwWritten;
    DWORD dwRes;
    BOOL fRes;

    // Create this write operation's OVERLAPPED structure's hEvent.
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (osWrite.hEvent == NULL) {
        printf("event error %d\n", GetLastError());
        // error creating overlapped event handle
        return FALSE;
    }

    // Issue write.
    printf("writing %s\n", lpBuf);
    if (!WriteFile(hComm, lpBuf, dwToWrite, &dwWritten, &osWrite)) {
        printf("looks like it pended %x\n", GetLastError());
        if (GetLastError() != ERROR_IO_PENDING) { 
            // WriteFile failed, but isn't delayed. Report error and abort.
            printf("not pending %d\n", GetLastError());
            fRes = FALSE;
        } else {
            // Write is pending.
            printf("write pended...\n");
            dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
            switch(dwRes) {
            // OVERLAPPED structure's event has been signaled. 
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE)) {
                    printf("not pending %d\n", GetLastError());
                    fRes = FALSE;
                } else {
                    // Write operation completed successfully.
                    fRes = TRUE;
                }
                break;
            
            default:
                // An error has occurred in WaitForSingleObject.
                // This usually indicates a problem with the
                // OVERLAPPED structure's event handle.
                printf("error occurred %d\n", GetLastError());
                fRes = FALSE;
                break;
            }
        }
    } else {
        printf("WriteFile completed immediately\n");
        fRes = TRUE;
    }

    CloseHandle(osWrite.hEvent);
    //printf("WriteABuffer returning %d\n", fRes);
    return fRes;
}

#define READ_TIMEOUT      500      // milliseconds
DWORD ReadABuffer(HANDLE hComm, char * lpBuf, DWORD dwToRead)
{
    DWORD dwRead;
    BOOL fWaitingOnRead = FALSE;
    OVERLAPPED osReader = {0};
    DWORD dwRes;
    BOOL fRes;

    memset(lpBuf, '\0', dwToRead);
    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    if (osReader.hEvent == NULL) {
       // Error creating overlapped event; abort.
       return 0;
    }

    if (!fWaitingOnRead) {
        // Issue read operation.
        if (!ReadFile(hComm, lpBuf, dwToRead, &dwRead, &osReader)) {
            if (GetLastError() != ERROR_IO_PENDING) {   // read not delayed?
                printf("error, non pending IO\n");
                // Error in communications; report it.
                return 0;
            } else {
                //printf("pending IO\n");
                fWaitingOnRead = TRUE;
                dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
                switch(dwRes) {
                // OVERLAPPED structure's event has been signaled. 
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(hComm, &osReader, &dwRead, FALSE)) {
                        printf("not pending %d\n", GetLastError());
                        fRes = FALSE;
                    } else {
                        // Write operation completed successfully.
                        fRes = TRUE;
                    }
                    break;
            
                default:
                    // An error has occurred in WaitForSingleObject.
                    // This usually indicates a problem with the
                    // OVERLAPPED structure's event handle.
                    //printf("error occurred %d\n", GetLastError());
                    fRes = FALSE;
                    break;
                }
          }
       }
       else {    
          // read completed immediately
          //printf("immediate IO\n");
          //HandleASuccessfulRead(lpBuf, dwRead);
        }
    }
    return dwRead;
}

bool init(HANDLE hComm, int Baud)
{
    bool success;
    DCB state;

    success = FlushFileBuffers(hComm);
    if (!success)
    {
        printf("Failed to flush serial port\n");
        return false;
    }

    state.DCBlength = sizeof(DCB);
    success = GetCommState(hComm, &state);
    if (!success) {
        printf("Failed to get serial settings\n");
        return false;
    }
 
    state.BaudRate = Baud;
    success = SetCommState(hComm, &state);
    if (!success) {
        printf("Failed to set serial settings\n");
        return false;
    }
    return true;
}

void loop(HANDLE hComm)
{
    char buffer[256];
    int len;
    while (1) {
        WriteABuffer(hComm, "\n", 1);
        if (len = ReadABuffer(hComm, buffer, 256)) {
            if (len) { //strlen(buffer)) {
                printf(">>>\n");
                hexdump(buffer, len);
                
            }
        }
        Sleep(1000);
        //printf(".");        
    }
}

int main()
{
    HANDLE hComm;
    hComm = CreateFileA("\\\\.\\COM3",
                        GENERIC_READ | GENERIC_WRITE, 
                        0, 
                        0, 
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        0);
    if (hComm == INVALID_HANDLE_VALUE) {
        int err = GetLastError();
        printf("dude you suck: %x\n", err);
    } else {
        printf("dude you rock\n");
    }

    if (init(hComm, BAUD)) {
        loop(hComm);
    }
    CloseHandle(hComm);
}
