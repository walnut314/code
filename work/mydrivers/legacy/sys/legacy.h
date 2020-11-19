#ifndef _LEGACY_H_
#define _LEGACY_H_

#include <ntddk.h>
#include "public.h"

typedef struct _DEVICE_EXTENSION
{
    PDEVICE_OBJECT DeviceObject;
    ULONG NtDeviceNumber;
    UCHAR DeviceStatus;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegisteryPath
    );

VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );

static
NTSTATUS
CreateDevice(
    IN PDRIVER_OBJECT DriveObject,
    IN ULONG NtDeviceNumber
    );

NTSTATUS
DriverDispatch(
    IN PDEVICE_OBJECT pDO,
    IN PIRP Irp
    );


#endif
