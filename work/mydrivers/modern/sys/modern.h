/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    modern.h

Abstract:

    Contains function prototypes and includes other neccessary header files.

Environment:

    Kernel mode only.

--*/

#include <ntddk.h>
#include <wdf.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <wdmsec.h> // for SDDLs
#include "public.h" // contains IOCTL definitions
#include "Trace.h" // contains macros for WPP tracing

#define NTDEVICE_NAME_STRING      L"\\Device\\MODERN"
#define SYMBOLIC_NAME_STRING     L"\\DosDevices\\MODERN"
#define POOL_TAG                   'ELIF'

typedef struct _CONTROL_DEVICE_EXTENSION {

    HANDLE   FileHandle; // Store your control data here

} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CONTROL_DEVICE_EXTENSION,
                                        ControlGetData)

//
// Following request context is used only for the method-neither ioctl case.
//
typedef struct _REQUEST_CONTEXT {

    WDFMEMORY InputMemoryBuffer;
    WDFMEMORY OutputMemoryBuffer;

} REQUEST_CONTEXT, *PREQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext)

//
// Device driver routine declarations.
//

DRIVER_INITIALIZE DriverEntry;

//
// Don't use EVT_WDF_DRIVER_DEVICE_ADD for ModernDeviceAdd even though 
// the signature is same because this is not an event called by the 
// framework.
//
NTSTATUS
ModernDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DRIVER_UNLOAD ModernEvtDriverUnload;

EVT_WDF_DEVICE_CONTEXT_CLEANUP ModernEvtDriverContextCleanup;
EVT_WDF_DEVICE_SHUTDOWN_NOTIFICATION ModernShutdown;

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL FileEvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_READ FileEvtIoRead;
EVT_WDF_IO_QUEUE_IO_WRITE FileEvtIoWrite;

EVT_WDF_IO_IN_CALLER_CONTEXT ModernEvtDeviceIoInCallerContext;
EVT_WDF_DEVICE_FILE_CREATE ModernEvtDeviceFileCreate;
EVT_WDF_FILE_CLOSE ModernEvtFileClose;

VOID
PrintChars(
    _In_reads_(CountChars) PCHAR BufferAddress,
    _In_ size_t CountChars
    );

#pragma warning(disable:4127)

