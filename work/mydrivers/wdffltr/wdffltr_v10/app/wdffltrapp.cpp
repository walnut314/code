/////////////////////////////////////////////////////////////////////////////////
//
//    (C) Copyright 1995 - 2006 OSR Open Systems Resources, Inc.
//    All Rights Reserved
//
//    This sofware is supplied for instructional purposes only.
//
//    OSR Open Systems Resources, Inc. (OSR) expressly disclaims any warranty
//    for this software.  THIS SOFTWARE IS PROVIDED  "AS IS" WITHOUT WARRANTY
//    OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION,
//    THE IMPLIED WARRANTIES OF MECHANTABILITY OR FITNESS FOR A PARTICULAR
//    PURPOSE.  THE ENTIRE RISK ARISING FROM THE USE OF THIS SOFTWARE REMAINS
//    WITH YOU.  OSR's entire liability and your exclusive remedy shall not
//    exceed the price paid for this material.  In no event shall OSR or its
//    suppliers be liable for any damages whatsoever (including, without
//    limitation, damages for loss of business profit, business interruption,
//    loss of business information, or any other pecuniary loss) arising out
//    of the use or inability to use this software, even if OSR has been
//    advised of the possibility of such damages.  Because some states/
//    jurisdictions do not allow the exclusion or limitation of liability for
//    consequential or incidental damages, the above limitation may not apply
//    to you.
//
//    OSR Open Systems Resources, Inc.
//    105 Route 101A Suite 19
//    Amherst, NH 03031  (603) 595-6500 FAX: (603) 595-6503
//    email bugs to: bugs@osr.com
//
//
//    MODULE:
//
//      wdffltrapp.cpp
//
//    ABSTRACT:
//
//      This file contains the all of the user mode code that interacts
//      with the WDF filter driver
//
//    AUTHOR(S):
//
//      OSR Open Systems Resources, Inc.
// 
//    REVISION:   
//
//
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <winioctl.h>

//
// Some stuff used in the shared header that isn't defined
//  for user mode...
//
typedef LONG NTSTATUS;

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


#include "..\inc\wdffltr_ioctl.h"

//////////////////////////
// FORWARD DECLARATIONS //
//////////////////////////

BOOL 
WINAPI 
ControlHandler(
    DWORD ControlType
    );

VOID
PrintRequest(
    PWDFFLTR_REQUEST Request
    );

//PCHAR
const char *
RequestTypeToString(
    REQUEST_TYPE Type
    );

/////////////
// GLOBALS //
/////////////
HANDLE WdfFltrDevice = INVALID_HANDLE_VALUE;

///////////////////////////////////////////////////////////////////////////////
//
//  main
//
//      Entry point for the application
//
//  INPUTS:
//
//      None.
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
//void __cdecl main(void) {
int main()
{

    WDFFLTR_REQUEST_BUFFER requestBuffer;
    DWORD                  bytesRet;

    //
    // Setup a handler for Ctrl+C, which we'll use
    //  to indicate that the app should terminate
    //
    if (!SetConsoleCtrlHandler(ControlHandler,
                               TRUE)) {

        printf("Couldn't set control handler - %d\n",
                GetLastError());
        return 0;

    }

    //
    // Open the filter device
    //
    printf("Opening the filter control device...\n");
    WdfFltrDevice = CreateFile("\\\\.\\OSRWdfFltr",
                               GENERIC_READ|GENERIC_WRITE,
                               0,
                               0,
                               OPEN_EXISTING,
                               FILE_FLAG_OVERLAPPED,
                               0);

    if (WdfFltrDevice == INVALID_HANDLE_VALUE) {
        printf("Couldn't open device - %d\n",
                GetLastError());
        return 0;

    }

    //
    // And tell the filter to start tracking
    //
    printf("Telling the control device to start tracking...\n");
    if (!DeviceIoControl(WdfFltrDevice,
                         IOCTL_WDFFLT_START_TRACK,
                         0,
                         0,
                         0,
                         0,
                         &bytesRet,
                         0)) {

        printf("Couldn't start tracking - %d\n",
                GetLastError());
        return 0;

    }

    printf("Requests should begin to print as they arrive...\n");
    SetConsoleTitle("Press Control+C to stop...");

    while(TRUE) {

        if (DeviceIoControl(WdfFltrDevice,
                            IOCTL_WDFFLT_GET_BUFFER,
                            0,
                            0,
                            &requestBuffer,
                            sizeof(requestBuffer),
                            &bytesRet,
                            0)) {

            for (ULONG i = 0; i < requestBuffer.ValidRequestCount; i++) {

                PrintRequest(&requestBuffer.WdfRequests[i]);

            }

        }
        Sleep(500);
    }

    return 0;

}


///////////////////////////////////////////////////////////////////////////////
//
//  ControlHandler
//
//      Handler for control events. We'll use this to 
//      stop the tracker when the user kills the app
//
//  INPUTS:
//
//      ControlType - The control event that occured. We ignore it..
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      FALSE, which means the event wasn't handled (and that the
//       app should die)
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
BOOL 
WINAPI 
ControlHandler(
    DWORD ControlType
    ) {

    DWORD bytesRet;

    if (WdfFltrDevice != INVALID_HANDLE_VALUE) {

        //
        // Cleanup...
        //
        if (!DeviceIoControl(WdfFltrDevice,
                         IOCTL_WDFFLT_START_TRACK,
                         0,
                         0,
                         0,
                         0,
                         &bytesRet,
                         0)) {

            printf("Couldn't stop tracking - %d\n",
                    GetLastError());

        }
    }
    return FALSE;


}

///////////////////////////////////////////////////////////////////////////////
//
//  PrintRequest
//
//
//      Helper routine to print a WDFFLTR_REQUEST to stdout
//
//
//  INPUTS:
//
//      Request - The WDFFLTR_REQUEST
//
//  OUTPUTS:
//
//      None, results go to stdout
//
//  RETURNS:
//
//      None
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
PrintRequest(
    PWDFFLTR_REQUEST Request
    ) {

    CHAR line[512];

    switch (Request->RequestType) {
    case WdfFltrReadRequest:

        if (!Request->ReadRequest.IsCompletion) {
            sprintf(line, "Read - BufferLen 0x%x\n",
                Request->ReadRequest.BufferLength);
        } else {
            sprintf(line, "Read Complete - Status 0x%x, Info 0x%p\n",
                Request->ReadRequest.CompletionStatus.Status,
                Request->ReadRequest.CompletionStatus.Information);
        }

        printf(line);
        return;
    case WdfFltrWriteRequest:
        sprintf(line, "Write - BufferLen 0x%x\n",
            Request->WriteRequest.BufferLength);
        printf(line);
        return;
    case WdfFltrDeviceControlRequest:
        sprintf(line, "DeviceControl - IOCTL 0x%x - IBufferLen 0x%x - "\
                      "OBufferLen 0x%x\n",
            Request->DeviceControlRequest.IoControlCode,
            Request->DeviceControlRequest.InputBufferLength,
            Request->DeviceControlRequest.OutputBufferLength);
        printf(line);
        return;
    case WdfFltrInternalDeviceControlRequest:
        sprintf(line, "InternalDeviceControl - IOCTL 0x%x - IBufferLen 0x%x"\
                      "OBufferLen 0x%x\n",
            Request->DeviceControlRequest.IoControlCode,
            Request->DeviceControlRequest.InputBufferLength,
            Request->DeviceControlRequest.OutputBufferLength);
        printf(line);
        return;
    case WdfFltrPnPRequest:
        sprintf(line, "PnP - MinorFunction 0x%x\n",
            Request->PnPRequest.MinorFunction);
        printf(line);
        return;
    default:
        break;
    }

    printf("Bogus entry\n");
    return;
}

//
// Helper routine to get a string literal for a
//  REQUEST_TYPE
//
//PCHAR
const char *
RequestTypeToString(
    REQUEST_TYPE Type
    ) {

    switch (Type) {

    case WdfFltrReadRequest:
        return (const char *) "WdfFltrReadRequest";
    case WdfFltrWriteRequest:
        return (const char *) "WdfFltrWriteRequest";
    case WdfFltrDeviceControlRequest:
        return (const char *) "WdfFltrDeviceControlRequest";
    case WdfFltrInternalDeviceControlRequest:
        return (const char *) "WdfFltrInternalDeviceControlRequest";
    case WdfFltrMaxRequestType:
        return (const char *) "WdfFltrMaxRequestTyp";
    default:
        break;
    }
    return (const char *) "Unknown";
}
