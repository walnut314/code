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
//      wdffltr.h
//
//    ABSTRACT:
//
//      Main include file for the WDF filter driver
//
//    AUTHOR(S):
//
//      OSR Open Systems Resources, Inc.
// 
//    REVISION:   
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __WDFFLTR_H__
#define __WDFFLTR_H__

extern "C" {
#include <ntddk.h>
#include <wdf.h>
}

#include "wdffltr_ioctl.h"


///////////////
// DBG STUFF //
///////////////
#if DBG
#define WdfFltrTrace(_MSG_) { \
        DbgPrint("WDFFLT!"__FUNCTION__ ": ");   \
        DbgPrint _MSG_;                         \
}
#else
#define WdfFltrTrace(__MSG__) { \
}
#endif

///////////////////////////
// STRUCTURE DEFINITIONS //
///////////////////////////
#define DEVICE_CONTEXT_MAGIC    0x98761234


//
// Our per evice context
//
typedef struct _DEVICE_CONTEXT {

    ULONG       MagicNumber;

    WDFIOTARGET TargetToSendRequestsTo;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

#define IS_DEVICE_CONTEXT(_DC_) (((_DC_)->MagicNumber) == DEVICE_CONTEXT_MAGIC)

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT,
                                   WdfFltrGetDeviceContext)


//
// We'll need to keep track of all of the 
//  WDFFLTR_REQUEST_BUFFER structures if the 
//  user mode app starts to get behind in its
//  collecting. We'll do this with a
//  WDFFLTR_REQUEST_BUFFER_TRACKER structure
//
typedef struct _WDFFLTR_REQUEST_BUFFER_TRACKER {

    LIST_ENTRY              RequestBuffers;
    BOOLEAN                 Tracking;
    ULONG                   PendingRequestBuffersCount;
    KSPIN_LOCK              RequestBuffersLock;
    PWDFFLTR_REQUEST_BUFFER ActiveBuffer;

}WDFFLTR_REQUEST_BUFFER_TRACKER, *PWDFFLTR_REQUEST_BUFFER_TRACKER;

//
// Maximum number of outstanding buffers to go to user
//  mode. We need this sot that we don't consume
//  all resources in the system
//
#define WDFFLTR_MAX_PENDING_BUFFERS 100

#define WdfFltrLockBufferTracker(_BT_, _I_) \
    KeAcquireSpinLock(&(_BT_)->RequestBuffersLock, (_I_));

#define WdfFltrUnLockBufferTracker(_BT_, _I_) \
    KeReleaseSpinLock(&(_BT_)->RequestBuffersLock, (_I_));


/////////////
// GLOBALS //
/////////////
extern WDFFLTR_REQUEST_BUFFER_TRACKER WdfFltrRequestBufferTracker;
extern LONG                           WdfFltrDeviceCount;
extern WDFDEVICE                      WdfFltrControlDevice;
extern BOOLEAN                        WdfFltrControlDeviceOpen;

//////////////////////////
// CONTROL DEVICE NAMES //
//////////////////////////
#define WDFFLTR_CONTROL_NAME      L"\\Device\\OSRWdfFltr"
#define WDFFLTR_CONTROL_LINK      L"\\DosDevices\\OSRWdfFltr"

////////////////
// PROTOTYPES //
////////////////


/////////////////
// WDFFLTR.CPP //
/////////////////
VOID
DriverUnload(
    IN WDFDRIVER  Driver
    );

NTSTATUS
WdfFltrEvtDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    );

NTSTATUS
WdfFltrCreateControlDevice(
    IN WDFDRIVER Driver
    );

NTSTATUS
WdfFltrQueryRemove(
    IN WDFDEVICE Device
    );

NTSTATUS
WdfFltrD0Entry(
    IN WDFDEVICE  Device,
    IN WDF_POWER_DEVICE_STATE  PreviousState
    );

NTSTATUS
WdfFltrD0Exit(
    IN WDFDEVICE  Device,
    IN WDF_POWER_DEVICE_STATE  PreviousState
    );

NTSTATUS
WdfFltrWdmPnp(
    WDFDEVICE Device,
    PIRP Irp
    );

VOID
WdfFltrRead(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  Length
    );

VOID
WdfFltrReadComplete(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS Params,
    IN WDFCONTEXT Context
    );

VOID
WdfFltrWrite(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  Length
    );

VOID
WdfFltrDeviceControl(
    IN WDFQUEUE  Queue,
    IN WDFREQUEST  Request,
    IN size_t  OutputBufferLength,
    IN size_t  InputBufferLength,
    IN ULONG  IoControlCode
    );

VOID
WdfFltrInternalDeviceControl(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t OutputBufferLength,
    IN size_t InputBufferLength,
    IN ULONG IoControlCode
    );


VOID
WdfFltrForwardRequest(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
    );

VOID
WdfFltrForwardRequestWithCompletion(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request,
    IN PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    IN WDFCONTEXT CompletionContext
    );

PCHAR 
WdfPowerStateToString(
    IN WDF_POWER_DEVICE_STATE PowerState
    );


////////////////
// APPINT.CPP //
////////////////
VOID
WdfFltrControlCreate(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request,
    IN WDFFILEOBJECT FileObject
    );

VOID
WdfFltrControlClose(
    IN WDFFILEOBJECT FileObject
    );

NTSTATUS
WdfFltrInitRequestBufferTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker
    );

NTSTATUS
WdfFltrStartRequestBufferTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker
    );

NTSTATUS
WdfFltrStopRequestBufferTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker
    );

VOID
WdfFltrAddRequestToTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker,
    IN PWDFFLTR_REQUEST WdfFltrRequest
    );

VOID
WdfFltrAppIntDeviceControl(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t OutputBufferLength,
    IN size_t InputBufferLength,
    IN ULONG IoControlCode
    );


#endif // #ifndef __WDFFLTR_H__