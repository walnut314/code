#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <windows.h>


// https://msdn.microsoft.com/en-us/library/ff802693.aspx
//

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
        //printf("looks like it pended %x\n", GetLastError());
        if (GetLastError() != ERROR_IO_PENDING) { 
            // WriteFile failed, but isn't delayed. Report error and abort.
            printf("not pending %d\n", GetLastError());
            fRes = FALSE;
        } else {
            // Write is pending.
            //printf("write pended...\n");
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
BOOL ReadABuffer(HANDLE hComm, char * lpBuf, DWORD dwToRead)
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
       return FALSE;
    }

    if (!fWaitingOnRead) {
        // Issue read operation.
        if (!ReadFile(hComm, lpBuf, dwToRead, &dwRead, &osReader)) {
            if (GetLastError() != ERROR_IO_PENDING) {   // read not delayed?
                printf("error, non pending IO\n");
                // Error in communications; report it.
                return FALSE;
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
          printf("immediate IO\n");
          //HandleASuccessfulRead(lpBuf, dwRead);
        }
    }
    return TRUE;
}

void init(HANDLE hComm)
{
    char buffer[256];
    printf("resetting bus pirate\n");
    WriteABuffer(hComm, "#\n", 2);
    Sleep(3000);
    return;
    //FlushFileBuffers(hComm);
    if (ReadABuffer(hComm, buffer, 256)) {
        printf("%s\n", buffer);
    }
    //WriteABuffer(hComm, "#\n\n",3);
    //Sleep(300);
    WriteABuffer(hComm, "m5\n",3);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    Sleep(300);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "P\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    Sleep(300);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "W\n",2);
    WriteABuffer(hComm, "\n\n",2);
    WriteABuffer(hComm, "\n\n",2);
    Sleep(300);

#if 0
    Sleep(1000);
    printf("setting mode\n");
    WriteABuffer(hComm, "m5\n\n\n", 5);
    Sleep(1000);
    printf("enabling power\n");
    WriteABuffer(hComm, "W\n", 2);
    Sleep(1000);
    printf("enabling pullups\n");
    WriteABuffer(hComm, "P\n", 2);
    Sleep(1000);
#endif
}

void loop(HANDLE hComm)
{
    char buffer[256];
    while (1) {
        WriteABuffer(hComm, "[r]", 3);
        if (ReadABuffer(hComm, buffer, 256)) {
            if (strlen(buffer)) {
                printf("%s\n", buffer);
            }
        }
        Sleep(1000);
        printf(".");        
    }
}

int main()
{
    HANDLE hComm;
    hComm = CreateFileA("\\\\.\\COM12",  
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
    init(hComm);
    loop(hComm);
    CloseHandle(hComm);
}
