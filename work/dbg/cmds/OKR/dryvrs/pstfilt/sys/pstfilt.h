/*++

Copyright (c) 2023  Intel Corporation

Module Name:

    pstfilt.h

Abstract:

    Contains function prototypes and includes other neccessary header files.

Environment:

    Kernel mode only.

--*/

#include <wdm.h>
#include <wdf.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <wdmsec.h> // for SDDLs
#include "public.h" // contains IOCTL definitions
#include "Trace.h" // contains macros for WPP tracing

ULONG IOCTL_YOU_ARE_INTERESTED_IN = (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 2048, METHOD_BUFFERED, FILE_ANY_ACCESS);

// Our per Device context
//
typedef struct _PST_DEVICE_CONTEXT {  // NOLINT(cppcoreguidelines-pro-type-member-init)
    WDFDEVICE       WdfDevice;
    // Other interesting stuff would go here
    //
} PST_DEVICE_CONTEXT, *PPST_DEVICE_CONTEXT;

// Context accessor function
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PST_DEVICE_CONTEXT,
                                   PstGetDeviceContext)

DRIVER_INITIALIZE DriverEntry;

// Don't use EVT_WDF_DRIVER_DEVICE_ADD for PstDeviceAdd even though 
// the signature is same because this is not an event called by the 
// framework.
//
NTSTATUS
PstDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DRIVER_DEVICE_ADD           PstEvtDeviceAdd;
EVT_WDF_IO_QUEUE_IO_READ            PstEvtRead;
EVT_WDF_IO_QUEUE_IO_WRITE           PstEvtWrite;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL  PstEvtDeviceControl;
EVT_WDF_REQUEST_COMPLETION_ROUTINE  PstCompletionCallback;

VOID
PstSendAndForget(IN WDFREQUEST Request, 
                 IN PPST_DEVICE_CONTEXT DevContext);

VOID
PstSendWithCallback(IN WDFREQUEST Request, 
                    IN PPST_DEVICE_CONTEXT DevContext);

VOID
PstCompletionCallback(IN WDFREQUEST                     Request,
                      IN WDFIOTARGET                    Target,
                      IN PWDF_REQUEST_COMPLETION_PARAMS Params,
                      IN WDFCONTEXT                     Context);


