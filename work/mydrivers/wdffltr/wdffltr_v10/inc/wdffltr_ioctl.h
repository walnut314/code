///////////////////////////////////////////////////////////////////////////////
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
//      wdffltr_ioctl.h
//
//    ABSTRACT:
//
//      Shared definitions between the WDF filter driver and its user mode
//      application
//
//    AUTHOR(S):
//
//      OSR Open Systems Resources, Inc.
// 
//    REVISION:   
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __WDFFLTR_IOCTL_H__
#define __WDFFLTR_IOCTL_H__


//
// The following value is arbitrarily chosen from the space defined by Microsoft
// as being "for non-Microsoft use"
//
#define FILE_DEVICE_WDFFTLR 0x0F59

//
// Device control codes - values between 2048 and 4095 arbitrarily chosen
//
#define IOCTL_WDFFLT_GET_BUFFER CTL_CODE(FILE_DEVICE_WDFFTLR,\
                                         2049,               \
                                         METHOD_OUT_DIRECT,  \
                                         FILE_READ_ACCESS)   \

#define IOCTL_WDFFLT_START_TRACK CTL_CODE(FILE_DEVICE_WDFFTLR,\
                                          2050,               \
                                          METHOD_BUFFERED,    \
                                          FILE_READ_ACCESS)   \

#define IOCTL_WDFFLT_STOP_TRACK CTL_CODE(FILE_DEVICE_WDFFTLR,\
                                         2051,               \
                                         METHOD_BUFFERED,    \
                                         FILE_READ_ACCESS)   \


///////////////////////////
// STRUCTURE DEFINITIONS //
///////////////////////////


//
// All of the following definitions are for use with 
//  IOCTL_WDFFLT_GET_BUFFER
// 

//
// Each entry in the request buffer structure is tagged
//  with a request type, which indicates the type of 
//  WDFREQUEST it represents
//
typedef enum _REQUEST_TYPE {

    WdfFltrReadRequest = 0x2000,
    WdfFltrWriteRequest,
    WdfFltrDeviceControlRequest,
    WdfFltrInternalDeviceControlRequest,
    WdfFltrPnPRequest,
    WdfFltrMaxRequestType

}REQUEST_TYPE;


//
// All of the request buffer entries inherit the entry
//  header (C structure inheritence, not C++ inheritence :))
//
typedef struct _WDFFLTR_REQUEST_ENTRY_HEADER {

    REQUEST_TYPE RequestType;

}WDFFLTR_REQUEST_ENTRY_HEADER, *PWDFFLTR_REQUEST_ENTRY_HEADER;


//
// The following structures are specific to each 
//  REQUEST_TYPE. Note that only the minimum
//  amount of information for each operation is
//  available, though expansion shouldn't be a 
//  problem
//


///////////
// READS //
///////////
typedef struct _WDFFLTR_READ_REQUEST {

    size_t          BufferLength;

    //
    // We also log read completions. Does this represent
    //  a completion?
    //
    BOOLEAN         IsCompletion;

    //
    // If (IsCompletion), then this has the completion
    //  status
    //
    IO_STATUS_BLOCK CompletionStatus;

}WDFFLTR_READ_REQUEST, *PWDFFLTR_READ_REQUEST;

////////////
// WRITES //
////////////
typedef struct _WDFFLTR_WRITE_REQUEST {

    size_t BufferLength;

}WDFFLTR_WRITE_REQUEST, *PWDFFLTR_WRITE_REQUEST;

///////////////////////////////
// [INTERNAL] DEVICE CONTROL //
///////////////////////////////
typedef struct _WDFFLTR_DEVICE_CONTROL_REQUEST {

    size_t InputBufferLength;

    size_t OutputBufferLength;

    ULONG  IoControlCode;

}WDFFLTR_DEVICE_CONTROL_REQUEST, *PWDFFLTR_DEVICE_CONTROL_REQUEST;

////////////////
// IRP_MJ_PNP //
////////////////
typedef struct _WDFFLTR_PNP_REQUEST {

    UCHAR  MinorFunction;

}WDFFLTR_PNP_REQUEST, *PWDFFLTR_PNP_REQUEST;


//
// The WDFFLTR_REQUEST is a union of the previous request
//  types. One knows where to look by keying off of 
//  WDFFLTR_REQUEST->RequestType.
//
typedef struct _WDFFLTR_REQUEST : WDFFLTR_REQUEST_ENTRY_HEADER {

    union {

        WDFFLTR_READ_REQUEST           ReadRequest;
        WDFFLTR_WRITE_REQUEST          WriteRequest;
        WDFFLTR_DEVICE_CONTROL_REQUEST DeviceControlRequest;
        WDFFLTR_PNP_REQUEST            PnPRequest;
    };

}WDFFLTR_REQUEST, *PWDFFLTR_REQUEST;

//
// Every time the user mode app calls into the 
//  driver with an IOCTL_WDFFLT_GET_BUFFER, it will specifiy 
//  one or more WDFFLTR_REQUEST_BUFFERs in its OutputBuffer.
//
// In this request buffer the user mode app will retrieve
//  0->WDFFLTR_REQUESTS_PER_BUFFER WDFFLTR_REQUEST structures
//  which describe I/O or PnP requests received by the 
//  filter driver.
//
#define WDFFLTR_REQUESTS_PER_BUFFER 0x200

typedef struct _WDFFLTR_REQUEST_BUFFER {
    LIST_ENTRY      ListEntry;
    ULONG           ValidRequestCount;
    WDFFLTR_REQUEST WdfRequests[WDFFLTR_REQUESTS_PER_BUFFER];
}WDFFLTR_REQUEST_BUFFER, *PWDFFLTR_REQUEST_BUFFER;

#endif