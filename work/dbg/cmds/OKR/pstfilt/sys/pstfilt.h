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

#define NTDEVICE_NAME_STRING    L"\\Device\\PstFilt"
#define SYMBOLIC_NAME_STRING    L"\\DosDevices\\PstFilt"

#define NUM_PNP_CALLBACKS       (5)

// Our per Device context
//
typedef struct _PST_DEVICE_CONTEXT {  // NOLINT(cppcoreguidelines-pro-type-member-init)
    WDFDEVICE       WdfDevice;
    PDEVICE_OBJECT  WdmDevice;
    PDEVICE_OBJECT  WdmTarget;
    PWDFDEVICE_INIT DeviceInit;
} PST_DEVICE_CONTEXT, *PPST_DEVICE_CONTEXT;

typedef struct _CONTROL_DEVICE_EXTENSION {
    HANDLE   FileHandle; // Store your control data here
} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CONTROL_DEVICE_EXTENSION,
                                        ControlGetData)

// Context accessor function
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PST_DEVICE_CONTEXT,
                                   PstGetDeviceContext)

typedef struct _GLOBAL_PST {
    int ndevs;
    BOOLEAN fail;
    KMUTEX  lock;
    PPST_DEVICE_CONTEXT devs[8];
} GLOBAL_PST, *PGLOBAL_PST;

DRIVER_INITIALIZE DriverEntry;

VOID
PstDriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );

VOID
PstEvtDriverUnload(
    IN WDFDRIVER Driver
    );

// Don't use EVT_WDF_DRIVER_DEVICE_ADD for PstDeviceAdd even though 
// the signature is same because this is not an event called by the 
// framework.
//
NTSTATUS
PstDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    );

NTSTATUS
PstControlDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL  FileEvtIoDeviceControl;

EVT_WDF_DRIVER_DEVICE_ADD           PstEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP      PstEvtDeviceContextCleanup;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL  PstEvtDeviceControl;
EVT_WDF_REQUEST_COMPLETION_ROUTINE  PstCompletionCallback;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS    PstEvtDeviceWdmIrpPreprocess;

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


