#include "legacy.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT, DriverEntry )
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
        )
{
    NTSTATUS status;
    //PDEVICE_OBJECT pDevObj;
    //PDEVICE_EXTENSION pDevExt;
    ULONG NtDeviceNumber;

    //UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    DbgBreakPoint();

    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE]          = DriverDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = DriverDispatch;
    DriverObject->MajorFunction[IRP_MJ_READ]            = DriverDispatch;
    DriverObject->MajorFunction[IRP_MJ_WRITE]           = DriverDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = DriverDispatch;

    NtDeviceNumber = 0;
    status = CreateDevice(DriverObject, NtDeviceNumber);

    return status;
}

static
NTSTATUS
CreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG NtDeviceNumber
    )
{
    NTSTATUS status;
    PDEVICE_OBJECT pDevObj;
    //PDEVICE_EXTENSION pDevExt;
    UNICODE_STRING deviceName;
    WCHAR deviceNameBuffer[XX_MAX_NAME_LENGTH];
    UNICODE_STRING linkName;
    WCHAR linkNameBuffer[XX_MAX_NAME_LENGTH];
    UNICODE_STRING number;
    WCHAR numberBuffer[10];

    number.Buffer = numberBuffer;
    number.MaximumLength = 10;

    deviceName.Buffer = deviceNameBuffer;
    deviceName.MaximumLength = XX_MAX_NAME_LENGTH;
    deviceName.Length = 0;
    RtlAppendUnicodeToString(
            &deviceName,
            XX_NT_DEVICE_NAME);

    number.Length = 0;
    RtlIntegerToUnicodeString(
            NtDeviceNumber,
            10, 
            &number);
    RtlAppendUnicodeStringToString(
            &deviceName,
            &number);

    status = IoCreateDevice(
            DriverObject,
            sizeof(DEVICE_EXTENSION),
            &deviceName,
            FILE_DEVICE_UNKNOWN,
            0,
            TRUE,
            &pDevObj);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    linkName.Buffer = linkNameBuffer;
    linkName.MaximumLength = XX_MAX_NAME_LENGTH;
    linkName.Length = 0;
    RtlAppendUnicodeToString(
            &linkName,
            XX_DOS_DEVICE_NAME);

    number.Length = 0;
    RtlIntegerToUnicodeString(
            NtDeviceNumber,
            10, 
            &number);
    RtlAppendUnicodeStringToString(
            &linkName,
            &number);

    status = IoCreateSymbolicLink(
            &linkName,
            &deviceName);

    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(pDevObj);
        return status;
    }

    return status;
}

VOID
DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);
    PDEVICE_OBJECT pDevObj;
    UNICODE_STRING linkName;
    WCHAR linkNameBuffer[XX_MAX_NAME_LENGTH];
    UNICODE_STRING number;
    WCHAR numberBuffer[10];
    ULONG NtDeviceNumber;

    DbgBreakPoint();

    NtDeviceNumber = 0;
    number.Buffer = numberBuffer;
    number.MaximumLength = 10;

    number.Length = 0;
    RtlIntegerToUnicodeString(
            NtDeviceNumber,
            10, 
            &number);

    linkName.Buffer = linkNameBuffer;
    linkName.MaximumLength = XX_MAX_NAME_LENGTH;
    linkName.Length = 0;
    RtlAppendUnicodeToString(
            &linkName,
            XX_DOS_DEVICE_NAME);

    RtlAppendUnicodeStringToString(
            &linkName,
            &number);

    IoDeleteSymbolicLink(&linkName);

    pDevObj = DriverObject->DeviceObject;

    IoDeleteDevice(pDevObj);
}

NTSTATUS
DriverDispatch(
    IN PDEVICE_OBJECT pDO,
    IN PIRP Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION IrpStack;
    ULONG ControlCode;
    UNREFERENCED_PARAMETER(pDO);

    DbgBreakPoint();
    
    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    ControlCode = IrpStack->MajorFunction;

    switch (ControlCode) {
        case IRP_MJ_CREATE:
            KdPrint(("--> IRP_MJ_CREATE\n"));
            break;
        case IRP_MJ_CLOSE:
            KdPrint(("--> IRP_MJ_CLOSE\n"));
            break;
        case IRP_MJ_READ:
            KdPrint(("--> IRP_MJ_READ\n"));
            break;
        case IRP_MJ_WRITE:
            KdPrint(("--> IRP_MJ_WRITE\n"));
            break;
        case IRP_MJ_DEVICE_CONTROL:
            KdPrint(("--> IRP_MJ_DEVICE_CONTROL\n"));
            break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

