/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    filter.h

Abstract:

    Contains structure definitions and function prototypes for filter driver.

Environment:

    Kernel mode


Author:


--*/

#include <ntddk.h>
#include <ntddstor.h>
#include <ntdddisk.h>
#include <wdf.h>
#include <wdmsec.h> // for SDDLs
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include "genkfilt_ioctl.h"

#if !defined(_FILTER_H_)
#define _FILTER_H_


#define DRIVERNAME "filter.sys: "


typedef struct _FILTER_EXTENSION {
    WDFIOTARGET IoTarget;
    ULONG       DeviceType;
    ULONG       SerialNo;
    LONG        DiskNumber;
    ULONG       Signature;
    BOOLEAN     Blocked;
    BOOLEAN     IsBootOrSystemDisk;
} FILTER_EXTENSION, *PFILTER_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FILTER_EXTENSION,
                                        FilterGetData)

#define NTDEVICE_NAME_STRING      L"\\Device\\filter"
#define SYMBOLIC_NAME_STRING      L"\\DosDevices\\filter"

typedef struct _CONTROL_DEVICE_EXTENSION {

    PVOID   ControlData; // Store your control data here

} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CONTROL_DEVICE_EXTENSION,
                                        ControlGetData)

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD FilterEvtDeviceAdd;
EVT_WDF_DRIVER_UNLOAD FilterEvtDriverUnload;
EVT_WDF_DEVICE_PREPARE_HARDWARE FilterEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE FilterEvtDeviceReleaseHardware;

EVT_WDF_DEVICE_CONTEXT_CLEANUP FilterEvtDeviceContextCleanup;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL FilterEvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL FilterControlIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_READ           FilterEvtIoReadWrite;

#ifdef IOCTL_INTERFACE
NTSTATUS
FilterCreateControlDevice(
    WDFDEVICE Device
    );

VOID
FilterDeleteControlDevice(
    WDFDEVICE Device
    );

#endif

NTSTATUS
GetBootSignature(
        );

PFILTER_EXTENSION
GetFilterExtDiskNumber(
    IN ULONG DiskNumber
    );

PFILTER_EXTENSION
GetFilterExtDiskSignature(
    IN ULONG DiskSignature
    );

VOID
FilterEvtCreateEvent();


#endif
