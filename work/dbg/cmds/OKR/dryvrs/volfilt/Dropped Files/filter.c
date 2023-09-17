/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    filter.c

Abstract:

    upper filter driver
Environment:

    Kernel mode


Author:
--*/

#include "filter.h"

//
// Collection object is used to store all the FilterDevice objects so
// that any event callback routine can easily walk thru the list and pick a
// specific instance of the device for filtering.
//
WDFCOLLECTION   FilterDeviceCollection;
WDFWAITLOCK     FilterDeviceCollectionLock;

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, FilterEvtDeviceAdd)
#pragma alloc_text (PAGE, FilterEvtDeviceContextCleanup)
#pragma alloc_text (PAGE, FilterEvtDevicePrepareHardware)
#pragma alloc_text (PAGE, FilterEvtDeviceReleaseHardware)

#endif

//
// Define IOCTL_INTERFACE in your sources file if  you want your
// app to have private interaction with the filter driver. Read KB Q262305
// for more information.
//

#ifdef IOCTL_INTERFACE

//
// ControlDevice provides a sideband communication to the filter from
// usermode. This is required if the filter driver is sitting underneath
// another driver that fails custom ioctls defined by the Filter driver.
// Since there is one control-device for all instances of the device the
// filter is attached to, we will store the device handle in a global variable.
//
WDFDEVICE       ControlDevice = NULL;

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, FilterEvtIoDeviceControl)
#pragma alloc_text (PAGE, FilterCreateControlDevice)
#pragma alloc_text (PAGE, FilterDeleteControlDevice)
#endif

#endif


BOOTDISK_INFORMATION BootDiskInfo;


VOID  FilterReinit(
    struct _DRIVER_OBJECT  *DriverObject,
    PVOID  Context,
    ULONG Count)
{
    PFILTER_EXTENSION   filterExt;
    DbgPrint("FilterReinit ...count %d\n", Count);
    GetBootSignature();
    if ((0 == BootDiskInfo.BootDeviceSignature) || (0 == BootDiskInfo.SystemDeviceSignature)) {
        IoRegisterBootDriverReinitialization(DriverObject, FilterReinit, NULL);
    } else {
        filterExt = GetFilterExtDiskSignature(BootDiskInfo.BootDeviceSignature);
        if (filterExt) {
            DbgPrint("starting monitor of disk %d\n", filterExt->DiskNumber);
            filterExt->Blocked = TRUE;
        }
        filterExt = GetFilterExtDiskSignature(BootDiskInfo.SystemDeviceSignature);
        if (filterExt) {
            DbgPrint("starting monitor of disk %d\n", filterExt->DiskNumber);
            filterExt->Blocked = TRUE;
        }
    }
}
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG   config;
    NTSTATUS            status;
    WDF_OBJECT_ATTRIBUTES colAttributes;
    WDFDRIVER   hDriver;

    KdPrint(("GenKFilt: Filter Driver.\n"));
    KdPrint(("Built %s %s\n", __DATE__, __TIME__));


    BootDiskInfo.BootDeviceSignature = 0;
    BootDiskInfo.SystemDeviceSignature = 0;

    IoRegisterBootDriverReinitialization(DriverObject, FilterReinit, NULL);
    
    //
    // Initiialize driver config to control the attributes that
    // are global to the driver. Note that framework by default
    // provides a driver unload routine. If you create any resources
    // in the DriverEntry and want to be cleaned in driver unload,
    // you can override that by manually setting the EvtDriverUnload in the
    // config structure. In general xxx_CONFIG_INIT macros are provided to
    // initialize most commonly used members.
    //

    WDF_DRIVER_CONFIG_INIT(
        &config,
        FilterEvtDeviceAdd
    );
    config.EvtDriverUnload = FilterEvtDriverUnload;

    //
    // Create a framework driver object to represent our driver.
    //
    status = WdfDriverCreate(DriverObject,
                            RegistryPath,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &config,
                            &hDriver);
    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfDriverCreate failed with status 0x%x\n", status));
    }

    //
    // Since there is only one control-device for all the instances
    // of the physical device, we need an ability to get to particular instance
    // of the device in our FilterEvtIoDeviceControlForControl. For that we
    // will create a collection object and store filter device objects.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&colAttributes);
    colAttributes.ParentObject = hDriver;

    status = WdfCollectionCreate(&colAttributes,
                                &FilterDeviceCollection);
    if (!NT_SUCCESS(status))
    {
        KdPrint( ("WdfCollectionCreate failed with status 0x%x\n", status));
        return status;
    }

    WDF_OBJECT_ATTRIBUTES_INIT(&colAttributes);
    colAttributes.ParentObject = hDriver;

    status = WdfWaitLockCreate(&colAttributes,
                                &FilterDeviceCollectionLock);
    if (!NT_SUCCESS(status))
    {
        KdPrint( ("WdfWaitLockCreate failed with status 0x%x\n", status));
        return status;
    }
   
    FilterEvtCreateEvent();

    return status;
}

PKEVENT gEvent;
HANDLE hEvent;

VOID
FilterEvtCreateEvent(
        )
{
    NTSTATUS Status;
    UNICODE_STRING NtEventName, DosEventName;
    DbgPrint("creating MBR event %ws\n", GENKFILT_EVENT_KERNEL_NAME);
    RtlInitUnicodeString(&NtEventName, GENKFILT_EVENT_KERNEL_NAME);
    gEvent = IoCreateSynchronizationEvent(&NtEventName, &hEvent);
    if (NULL == gEvent) {
        DbgPrint("failed to create the event ...\n");
    } else {
        RtlInitUnicodeString(&DosEventName, GENKFILT_EVENT_LINK_NAME);
        Status = IoCreateSymbolicLink(&DosEventName, &NtEventName);
        if (NT_SUCCESS(Status)) {        
            DbgPrint("success creating symlink to event...\n");
            KeResetEvent(gEvent);
        } else {
            DbgPrint("failed to create symlink to event...%x\n", Status);
        }
    }
}

VOID
FilterEvtDriverUnload(
    IN WDFDRIVER        Driver
    )
{
    return;
}

ULONG device_number = 1;
void dumpdeviceinfo(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
{
    DbgPrint("device number %d\n", device_number);
    DbgPrint(" device_init %x\n",  DeviceInit);
    device_number++;
}


NTSTATUS
FilterEvtDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. Here you can query the device properties
    using WdfFdoInitWdmGetPhysicalDevice/IoGetDeviceProperty and based
    on that, decide to create a filter device object and attach to the
    function stack. If you are not interested in filtering this particular
    instance of the device, you can just return STATUS_SUCCESS without creating
    a framework device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES           deviceAttributes;
    WDF_PNPPOWER_EVENT_CALLBACKS    pnpPowerCallbacks;
    WDF_IO_QUEUE_CONFIG             queueConfig;
    PFILTER_EXTENSION               filterExt;
    NTSTATUS                        status;
    WDFDEVICE                       device;
    ULONG                           serialNo;
    ULONG                           returnSize;
    WDFQUEUE                        queue;
    PDEVICE_OBJECT                  pdo = NULL;

    DECLARE_CONST_UNICODE_STRING(ntDeviceName, NTDEVICE_NAME_STRING) ;
    DECLARE_CONST_UNICODE_STRING(symbolicLinkName, SYMBOLIC_NAME_STRING) ;

    PAGED_CODE ();

    UNREFERENCED_PARAMETER(Driver);

    //
    // Initialize the pnpPowerCallbacks structure.  Callback events for PNP
    // and Power are specified here.  If you don't supply any callbacks,
    // the Framework will take appropriate default actions based on whether
    // DeviceInit is initialized to be an FDO, a PDO or a filter device
    // object.
    //

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    pnpPowerCallbacks.EvtDevicePrepareHardware = FilterEvtDevicePrepareHardware;
    pnpPowerCallbacks.EvtDeviceReleaseHardware = FilterEvtDeviceReleaseHardware;

    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);    

    dumpdeviceinfo(Driver, DeviceInit);

    //
    // Get some property of the device you are about to attach and check
    // to see if that's the one you are interested. For demonstration
    // we will get the UINumber of the device. The bus driver reports the
    // serial number as the UINumber.
    //
/*    
    status = WdfFdoInitQueryProperty(DeviceInit,
                                  DevicePropertyUINumber,
                                  sizeof(serialNo),
                                  &serialNo,
                                  &returnSize);
    if(!NT_SUCCESS(status)){
        KdPrint(("Failed to get the property of PDO: 0x%p\n", DeviceInit));

    }
*/
    //
    // Tell the framework that you are filter driver. Framework
    // takes care of inherting all the device flags & characterstics
    // from the lower device you are attaching to.
    //
    WdfFdoInitSetFilter(DeviceInit);

    //
    // Specify the size of device extension where we track per device
    // context.
    //

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, FILTER_EXTENSION);

    //
    // We will just register for cleanup notification because we have to
    // delete the control-device when the last instance of the device goes
    // away. If we don't delete, the driver wouldn't get unloaded automatcially
    // by the PNP subsystem.
    //
    deviceAttributes.EvtCleanupCallback = FilterEvtDeviceContextCleanup;

    //
    // Create a framework device object.This call will inturn create
    // a WDM deviceobject, attach to the lower stack and set the
    // appropriate flags and attributes.
    //
    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfDeviceCreate failed with status code 0x%x\n", status));
        return status;
    }

    //
    // Add this device to the FilterDevice collection.
    //
    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);
    status = WdfCollectionAdd(FilterDeviceCollection, device);
    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfCollectionAdd failed with status code 0x%x\n", status));
    }
    WdfWaitLockRelease(FilterDeviceCollectionLock);

    filterExt = FilterGetData(device);
    filterExt->Blocked = FALSE;
    filterExt->DiskNumber = -1;
    filterExt->Signature = 0;
    filterExt->SerialNo = -2;
    filterExt->IsBootOrSystemDisk = FALSE;
    filterExt->IoTarget = WdfDeviceGetIoTarget(device);
    
    //
    // Create a control device if IOCTL_INTERFACE is defined in the sources file.
    //
#ifdef IOCTL_INTERFACE
    pdo = WdfDeviceWdmGetPhysicalDevice(device);
    if (pdo) {
        filterExt->DeviceType = pdo->DeviceType;
        if (pdo->DeviceType == FILE_DEVICE_CONTROLLER) {
            DbgPrint("need to create control name here...%x\n", pdo);
            status = FilterCreateControlDevice(device);
            if (!NT_SUCCESS(status)) {
                KdPrint( ("FilterCreateControlDevice failed with status 0x%x\n", status));
                //
                // Let us not fail AddDevice just because we weren't able to create the
                // control device.
                //
                status = STATUS_SUCCESS;
            }
        }
        
    }


#endif

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
    queueConfig.EvtIoDeviceControl  = FilterEvtIoDeviceControl;
    queueConfig.EvtIoRead           = FilterEvtIoReadWrite;
    queueConfig.EvtIoWrite          = FilterEvtIoReadWrite;
    status = WdfIoQueueCreate( device,
                               &queueConfig,
                               WDF_NO_OBJECT_ATTRIBUTES,
                               &queue );

    if (!NT_SUCCESS(status)) {
        KdPrint(("LEAVE:%s : line %d status 0x%0x\n",__FUNCTION__, __LINE__,status));
        return status;
    }


    return status;
}

VOID GetDiskNumber(
    IN PFILTER_EXTENSION FilterExt
        )
{
    STORAGE_DEVICE_NUMBER   number;
    NTSTATUS                status;
    WDFIOTARGET             IoTarget;
    DWORD                   bytesReturned;
    WDF_MEMORY_DESCRIPTOR   outputDesc;
    PWDF_MEMORY_DESCRIPTOR  pOutputDesc = NULL;

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDesc,
                                      &number,
                                      sizeof(number));
    pOutputDesc = &outputDesc;

    if ((FilterExt->DeviceType != FILE_DEVICE_MASS_STORAGE) || (FilterExt->DiskNumber > 0)) {
        return;
    }
    IoTarget = FilterExt->IoTarget;

    status = WdfIoTargetSendIoctlSynchronously(
                        IoTarget,
                        WDF_NO_HANDLE, // Request
                        IOCTL_STORAGE_GET_DEVICE_NUMBER,
                        NULL,
                        pOutputDesc,
                        NULL, // PWDF_REQUEST_SEND_OPTIONS
                        &bytesReturned);


    if (!NT_SUCCESS(status)) {
        DbgPrint("GetDiskNumber - failed 0x%x\n", status);
    } else {
        DbgPrint("GetDiskNumber succeeded - disk number %d\n", number.DeviceNumber);
        FilterExt->DiskNumber = number.DeviceNumber;
    }
}

NTSTATUS
GetBootSignature(
        )
{
    NTSTATUS Status;

    Status = IoGetBootDiskInformation(&BootDiskInfo, sizeof(BOOTDISK_INFORMATION));
    if (NT_SUCCESS(Status)) {
        DbgPrint("GetBootSignature success...:\n");
        DbgPrint("BootDeviceSignature   %x\n", BootDiskInfo.BootDeviceSignature);
        DbgPrint("SystemDeviceSignature %x\n", BootDiskInfo.SystemDeviceSignature);
    } else {
        DbgPrint("BootDeviceSignature failed---: %x\n", Status);
    }

    return Status;
}

NTSTATUS
GetDiskSignature(
    IN PFILTER_EXTENSION FilterExt
        )
{
    NTSTATUS                status = STATUS_UNSUCCESSFUL;
    WDFIOTARGET             IoTarget;
    DWORD                   bytesReturned;
    WDF_MEMORY_DESCRIPTOR   outputDesc;
    PWDF_MEMORY_DESCRIPTOR  pOutputDesc = NULL;
    DRIVE_LAYOUT_INFORMATION layout, *pLayout = NULL;
    size_t                  size;

    size = (sizeof(DRIVE_LAYOUT_INFORMATION) + (32 * sizeof(PARTITION_INFORMATION)));

    pOutputDesc = &outputDesc;
    pLayout = ExAllocatePoolWithTag(NonPagedPool, size, 'fdmK');
    
    //WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDesc,
    //                                  &layout,
    //                                  sizeof(layout));
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDesc,
                                      pLayout,
                                      size);

    if (FilterExt->DeviceType != FILE_DEVICE_MASS_STORAGE) {
        goto out;
    }
    IoTarget = FilterExt->IoTarget;

    status = WdfIoTargetSendIoctlSynchronously(
                        IoTarget,
                        WDF_NO_HANDLE, // Request
                        IOCTL_DISK_GET_DRIVE_LAYOUT,
                        NULL,
                        pOutputDesc,
                        NULL, // PWDF_REQUEST_SEND_OPTIONS
                        &bytesReturned);


    if (!NT_SUCCESS(status)) {
        DbgPrint("GetDiskSignature - failed 0x%x\n", status);
    } else {
        DbgPrint("GetDiskSignature succeeded - disk signature %x\n", pLayout->Signature);
        FilterExt->Signature = pLayout->Signature;
    }

out:
    if (pLayout) {
        ExFreePool(pLayout);
        pLayout = NULL;
    }

    return status;
}

VOID
FilterEvtIoReadWrite(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t        Length
    )
{
    WDFDEVICE   hDevice;
    BOOLEAN     ret;
    WDF_REQUEST_SEND_OPTIONS options;    
    NTSTATUS status;
    PIRP  Irp = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    LARGE_INTEGER Location;
    PFILTER_EXTENSION   filterExt = NULL;

    hDevice = WdfIoQueueGetDevice(Queue);
    filterExt = FilterGetData(WdfIoQueueGetDevice(Queue));

    Irp = WdfRequestWdmGetIrp(Request);
    if (NULL == Irp) {
        KdPrint(("FilterEvtIoReadWrite: error retrieving IRP\n"));
        goto FilterEvtIoReadWrite_continue;
    }
    irpStack = IoGetCurrentIrpStackLocation(Irp);
    if (NULL == irpStack) {
        KdPrint(("FilterEvtIoReadWrite: error retrieving IRP stack\n"));
        goto FilterEvtIoReadWrite_continue;
    }
    switch (irpStack->MajorFunction) {
        case IRP_MJ_READ:
            Location = irpStack->Parameters.Read.ByteOffset;
            if (Location.QuadPart < 512 && Location.QuadPart >= 0) {
                if (filterExt->Blocked) {
                    DbgPrint("detected read from MBR ... setting event\n");
                    //KeSetEvent(gEvent, IO_NO_INCREMENT, FALSE);
                    //status = STATUS_SUCCESS;
                    //WdfRequestComplete(Request, status);
                    //return;
                }
            }
            break;
        case IRP_MJ_WRITE:
            Location = irpStack->Parameters.Write.ByteOffset;
            if (Location.QuadPart < 512 && Location.QuadPart >= 0) {
                if (filterExt->Blocked) {
                    DbgPrint("detected read from MBR ... setting event\n");
                    //KeSetEvent(gEvent, IO_NO_INCREMENT, FALSE);
                    //status = STATUS_SUCCESS;
                    //WdfRequestComplete(Request, status);
                    //return;
                }
            }
            break;
        default:
            DbgPrint("unknown\n");
            break;
    }

FilterEvtIoReadWrite_continue:
    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&options,
                                  WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(hDevice), &options);
    
    if (ret == FALSE) {
        status = WdfRequestGetStatus (Request);
        KdPrint( ("WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }

    return;
}

VOID
FilterEvtIoDeviceControl(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t           OutputBufferLength,
    IN size_t           InputBufferLength,
    IN ULONG            IoControlCode
    )
/*++
Routine Description:

    This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
    requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    WDFDEVICE   hDevice;
    BOOLEAN     ret;
    WDF_REQUEST_SEND_OPTIONS options;    
    NTSTATUS status;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);


    hDevice = WdfIoQueueGetDevice(Queue);
    
    //KdPrint(("Ioctl recieved into filter control object.\n"));
    
    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&options,
                                  WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(hDevice), &options);
    
    if (ret == FALSE) {
        status = WdfRequestGetStatus (Request);
        KdPrint( ("WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }

    return;
}

PFILTER_EXTENSION
GetFilterExtDiskNumber(
    IN ULONG DiskNumber
    )
{
    ULONG               noItems, i;
    WDFDEVICE           hFilterDevice;
    PFILTER_EXTENSION   filterExt = NULL;

    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);
    noItems = WdfCollectionGetCount(FilterDeviceCollection);
    WdfWaitLockRelease(FilterDeviceCollectionLock);

    for(i = 0; i < noItems; i++) {
        hFilterDevice = WdfCollectionGetItem(FilterDeviceCollection, i);
        filterExt = FilterGetData(hFilterDevice);
        if (filterExt->DiskNumber == DiskNumber) {
            break;
        }
    }
    return filterExt;
}

PFILTER_EXTENSION
GetFilterExtDiskSignature(
    IN ULONG DiskSignature
    )
{
    ULONG               noItems, i;
    WDFDEVICE           hFilterDevice;
    PFILTER_EXTENSION   filterExt = NULL;

    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);
    noItems = WdfCollectionGetCount(FilterDeviceCollection);
    WdfWaitLockRelease(FilterDeviceCollectionLock);

    for(i = 0; i < noItems; i++) {
        hFilterDevice = WdfCollectionGetItem(FilterDeviceCollection, i);
        filterExt = FilterGetData(hFilterDevice);
        if (filterExt->Signature == DiskSignature) {
            break;
        }
    }
    return filterExt;
}

VOID
FilterControlIoDeviceControl(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t           OutputBufferLength,
    IN size_t           InputBufferLength,
    IN ULONG            IoControlCode
    )
/*++
Routine Description:

    This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
    requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    WDFDEVICE   hDevice;
    BOOLEAN     ret;
    WDF_REQUEST_SEND_OPTIONS options;    
    NTSTATUS status = STATUS_SUCCESS;
    ULONG               i;
    ULONG               noItems;
    WDFDEVICE           hFilterDevice;
    PFILTER_EXTENSION   filterExt;
    size_t              bytesReturned = 0;
    size_t              size = 0;
    PQUERY_SETTINGS     query = NULL;
    PGENK_DEV           set = NULL;
    PVOID               inBuffer = NULL, outBuffer = NULL;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);


    hDevice = WdfIoQueueGetDevice(Queue);
    
    KdPrint(("Ioctl recieved into filter control object.\n"));
    switch (IoControlCode) {
        case IOCTL_GENKFILT_RESET:
            DbgPrint("received IOCTL_GENKFILT_RESET...\n");
            KeResetEvent(gEvent);
            break;
        case IOCTL_GENKFILT_QUERY_SETTINGS:
            DbgPrint("received IOCTL_GENKFILT_QUERY_SETTINGS...\n");
            WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);
            noItems = WdfCollectionGetCount(FilterDeviceCollection);
            WdfWaitLockRelease(FilterDeviceCollectionLock);
            status = WdfRequestRetrieveOutputBuffer(Request,
                                                    OutputBufferLength,
                                                    &outBuffer,
                                                    &size);
            if (NT_SUCCESS(status)) {
                if (OutputBufferLength < sizeof(QUERY_SETTINGS) + noItems * sizeof(GENK_DEV)) {
                    size  = (USHORT) (sizeof(QUERY_SETTINGS) + noItems * sizeof(GENK_DEV));
                    *(PUSHORT)outBuffer = (USHORT) size;
                    bytesReturned = sizeof(USHORT);
                    DbgPrint("returning STATUS_BUFFER_TOO_SMALL len %d need %d\n", OutputBufferLength, size);
                    status = STATUS_BUFFER_TOO_SMALL;
                } else {
                    query = (PQUERY_SETTINGS) outBuffer;
                    query->NumItems = 0;
                    for(i = 0; i < noItems; i++) {
                        hFilterDevice = WdfCollectionGetItem(FilterDeviceCollection, i);
                        filterExt = FilterGetData(hFilterDevice);
                        GetDiskNumber(filterExt);
                        if (filterExt->DeviceType == FILE_DEVICE_MASS_STORAGE) {
                            query->dev[query->NumItems].DeviceType  = filterExt->DeviceType;
                            query->dev[query->NumItems].SerialNo    = filterExt->SerialNo;
                            query->dev[query->NumItems].DiskNumber  = filterExt->DiskNumber;
                            query->dev[query->NumItems].Blocked     = filterExt->Blocked;
                            query->NumItems++;
                        }
                    }
                    bytesReturned = (USHORT) (sizeof(QUERY_SETTINGS) + query->NumItems * sizeof(GENK_DEV));
                    DbgPrint("returning STATUS_SUCCESS len %d\n", bytesReturned);
                    status = STATUS_SUCCESS;                    
                }
            }
            //WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 0);
            break;

        case IOCTL_GENKFILT_SET:
            DbgPrint("received IOCTL_GENKFILT_SET...\n");
            status = WdfRequestRetrieveInputBuffer(Request,
                                                   InputBufferLength,
                                                   &inBuffer,
                                                   &size);
            if (NT_SUCCESS(status)) {
                if (InputBufferLength < sizeof(GENK_DEV)) {
                    DbgPrint("returning STATUS_BUFFER_TOO_SMALL \n");
                    status = STATUS_BUFFER_TOO_SMALL;
                } else {
                    // get filter ext for disk number
                    set = (PGENK_DEV) inBuffer;
                    //filterExt = GetFilterExtDiskNumber(set->DiskNumber);
                    filterExt = GetFilterExtDiskSignature(BootDiskInfo.BootDeviceSignature);
                    if (filterExt) {
                        DbgPrint("setting disk %d to %d \n", filterExt->DiskNumber, set->Blocked);
                        filterExt->Blocked = set->Blocked;
                    }
                    filterExt = GetFilterExtDiskSignature(BootDiskInfo.SystemDeviceSignature);
                    if (filterExt) {
                        DbgPrint("setting disk %d to %d \n", filterExt->DiskNumber, set->Blocked);
                        filterExt->Blocked = set->Blocked;
                    }
                    status = STATUS_SUCCESS;
                }
            }
            bytesReturned = 0;
            break;
        default:
            status = STATUS_SUCCESS;
            bytesReturned = 0;
            DbgPrint("received unknown IOCTL...%x\n", IoControlCode);
            break;
    }
    WdfRequestCompleteWithInformation(Request, status, bytesReturned);
    //WdfRequestComplete(Request, status);
    return;
    
    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&options,
                                  WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    ret = WdfRequestSend(Request, WdfDeviceGetIoTarget(hDevice), &options);
    
    if (ret == FALSE) {
        status = WdfRequestGetStatus (Request);
        KdPrint( ("WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }

    return;
}



VOID
FilterEvtDeviceContextCleanup(
    IN WDFDEVICE Device
    )
/*++

Routine Description:

   EvtDeviceRemove event callback must perform any operations that are
   necessary before the specified device is removed. The framework calls
   the driver's EvtDeviceRemove callback when the PnP manager sends
   an IRP_MN_REMOVE_DEVICE request to the driver stack.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    WDF status code

--*/
{
#ifdef IOCTL_INTERFACE
    ULONG   count;
#endif
    PAGED_CODE();

    KdPrint(("Entered FilterEvtDeviceContextCleanup\n"));

    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);

#ifdef IOCTL_INTERFACE
    count = WdfCollectionGetCount(FilterDeviceCollection);
        if(count == 1)
        {
            //
            // We are the last instance. So let us delete the control-device
            // so that driver can unload when the FilterDevice is deleted.
            // We absolutely have to do the deletion of control device with
            // the collection lock acquired because we implicitly use this
            // lock to protect ControlDevice global variable. We need to make
            // sure another thread doesn't attempt to create while we are
            // deleting the device.
            //
            FilterDeleteControlDevice(Device);
        }

#endif

    WdfCollectionRemove(FilterDeviceCollection, Device);

    WdfWaitLockRelease(FilterDeviceCollectionLock);
}

#ifdef IOCTL_INTERFACE

NTSTATUS
FilterCreateControlDevice(
    WDFDEVICE Device
    )
/*++

Routine Description:

    This routine is called to create a control deviceobject so that application
    can talk to the filter driver directly instead of going through the entire
    device stack. This kind of control device object is useful if the filter
    driver is underneath another driver which prevents ioctls not known to it
    or if the driver's dispatch routine is owned by some other (port/class)
    driver and it doesn't allow any custom ioctls.

    NOTE: Since the control device is global to the driver and accessible to
    all instances of the device this filter is attached to, we create only once
    when the first instance of the device is started and delete it when the
    last instance gets removed.

Arguments:

    Device - Handle to a filter device object.

Return Value:

    WDF status code

--*/
{
    PWDFDEVICE_INIT             pInit = NULL;
    WDFDEVICE                   controlDevice = NULL;
    WDF_OBJECT_ATTRIBUTES       controlAttributes;
    WDF_IO_QUEUE_CONFIG         ioQueueConfig;
    BOOLEAN                     bCreate = FALSE;
    NTSTATUS                    status;
    WDFQUEUE                    queue;
    DECLARE_CONST_UNICODE_STRING(ntDeviceName, NTDEVICE_NAME_STRING) ;
    DECLARE_CONST_UNICODE_STRING(symbolicLinkName, SYMBOLIC_NAME_STRING) ;

    PAGED_CODE();

    //
    // First find out whether any ControlDevice has been created. If the
    // collection has more than one device then we know somebody has already
    // created or in the process of creating the device.
    //
#if 0
    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);

    if(WdfCollectionGetCount(FilterDeviceCollection) == 1) {
        bCreate = TRUE;
    }

    WdfWaitLockRelease(FilterDeviceCollectionLock);

    if(!bCreate) {
        //
        // Control device is already created. So return success.
        //
        return STATUS_SUCCESS;
    }
#endif

    //
    //
    // In order to create a control device, we first need to allocate a
    // WDFDEVICE_INIT structure and set all properties.
    //
    pInit = WdfControlDeviceInitAllocate(
                            WdfDeviceGetDriver(Device),
                            &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R
                            );

    if (pInit == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Error;
    }

    //
    // Set exclusive to false so that more than one app can talk to the
    // control device simultaneously.
    //
    WdfDeviceInitSetExclusive(pInit, FALSE);

    status = WdfDeviceInitAssignName(pInit, &ntDeviceName);

    if (!NT_SUCCESS(status)) {
        goto Error;
    }

    //
    // Specify the size of device context
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&controlAttributes,
                                    CONTROL_DEVICE_EXTENSION);
    KdPrint(("Creating Control Device %ws\n", ntDeviceName.Buffer));
    status = WdfDeviceCreate(&pInit,
                             &controlAttributes,
                             &controlDevice);
    if (!NT_SUCCESS(status)) {
        goto Error;
    }

    //
    // Create a symbolic link for the control object so that usermode can open
    // the device.
    //

    status = WdfDeviceCreateSymbolicLink(controlDevice,
                                &symbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto Error;
    }

    //
    // Configure the default queue associated with the control device object
    // to be Serial so that request passed to EvtIoDeviceControl are serialized.
    //

#if 1
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                             WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoDeviceControl = FilterControlIoDeviceControl;

    //
    // Framework by default creates non-power managed queues for
    // filter drivers.
    //
    status = WdfIoQueueCreate(controlDevice,
                                        &ioQueueConfig,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &queue // pointer to default queue
                                        );
    if (!NT_SUCCESS(status)) {
        goto Error;
    }
#endif
    //
    // Control devices must notify WDF when they are done initializing.   I/O is
    // rejected until this call is made.
    //
    WdfControlFinishInitializing(controlDevice);

    ControlDevice = controlDevice;

    return STATUS_SUCCESS;

Error:

    if (pInit != NULL) {
        WdfDeviceInitFree(pInit);
    }

    if (controlDevice != NULL) {
        //
        // Release the reference on the newly created object, since
        // we couldn't initialize it.
        //
        WdfObjectDelete(controlDevice);
        controlDevice = NULL;
    }

    return status;
}

VOID
FilterDeleteControlDevice(
    WDFDEVICE Device
    )
/*++

Routine Description:

    This routine deletes the control by doing a simple dereference.

Arguments:

    Device - Handle to a framework filter device object.

Return Value:

    WDF status code

--*/
{
    UNREFERENCED_PARAMETER(Device);

    PAGED_CODE();

    KdPrint(("Deleting Control Device\n"));

    if (ControlDevice) {
        WdfObjectDelete(ControlDevice);
        ControlDevice = NULL;
    }
}

#if 0
VOID
FilterEvtIoDeviceControl(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t           OutputBufferLength,
    IN size_t           InputBufferLength,
    IN ULONG            IoControlCode
    )
/*++
Routine Description:

    This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
    requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    ULONG               i;
    ULONG               noItems;
    WDFDEVICE           hFilterDevice;
    PFILTER_EXTENSION   filterExt;

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(IoControlCode);

    PAGED_CODE();

    KdPrint(("Ioctl recieved into filter control object.\n"));

    WdfWaitLockAcquire(FilterDeviceCollectionLock, NULL);

    noItems = WdfCollectionGetCount(FilterDeviceCollection);

    for(i=0; i<noItems ; i++) {

        hFilterDevice = WdfCollectionGetItem(FilterDeviceCollection, i);

        filterExt = FilterGetData(hFilterDevice);

        KdPrint(("Serial No: %d\n", filterExt->SerialNo));
    }

GetBootSignature
    WdfWaitLockRelease(FilterDeviceCollectionLock);


    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 0);
}
#endif

#endif

NTSTATUS
FilterEvtDevicePrepareHardware(
    IN WDFDEVICE Device,
    IN WDFCMRESLIST ResourceList,
    IN WDFCMRESLIST ResourceListTranslated
    )
{
    PFILTER_EXTENSION filterExt;
    NTSTATUS Status;

    filterExt = FilterGetData(Device);
    DbgPrint("FilterEvtDevicePrepareHardware: \n");

    //GetBootSignature();

    GetDiskNumber(filterExt);
    GetDiskSignature(filterExt);

    return STATUS_SUCCESS;
}

NTSTATUS
FilterEvtDeviceReleaseHardware(
    IN WDFDEVICE Device,
    IN WDFCMRESLIST ResourceListTranslated
    )
{
    PFILTER_EXTENSION filterExt;
    filterExt = FilterGetData(Device);
    DbgPrint("FilterEvtDeviceReleaseHardware: \n");
    return STATUS_SUCCESS;
}

