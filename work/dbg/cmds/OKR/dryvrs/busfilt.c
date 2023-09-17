#include "busfilt.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, gfFdoAddDevice)
#pragma alloc_text (PAGE, gfDispatchPnp)
#pragma alloc_text (PAGE, gfDispatchPower)
#pragma alloc_text (PAGE, gfUnload)
#endif

#define POOL_TAG 'rtlF'
GDATA  g_data;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS          status = STATUS_SUCCESS;
    ULONG             ulIndex;
    PDRIVER_DISPATCH *dispatch;
    PDEVICE_OBJECT    deviceObject = NULL;
    UNICODE_STRING    usNTName, usDosName;
    PDEVICE_EXTENSION deviceExtension; 

    UNREFERENCED_PARAMETER (RegistryPath);

    KPRINT(("entered GenFilt DriverEntry\n"));

    RtlInitUnicodeString(&usNTName, NT_DEVICE_NAME);

    status = IoCreateDevice(
                    DriverObject,
                    sizeof (DEVICE_EXTENSION),
                    &usNTName,
                    FILE_DEVICE_UNKNOWN,
                    0,
                    FALSE,
                    &deviceObject
                    );
    
    if (NT_SUCCESS(status)) {
        //
        // Create dispatch points
        //
        for (ulIndex = 0, dispatch = DriverObject->MajorFunction;
             ulIndex <= IRP_MJ_MAXIMUM_FUNCTION;
             ulIndex++, dispatch++) {

            *dispatch = gfPass;
        }

        //DriverObject->MajorFunction[IRP_MJ_CREATE]          = gfCreateClose;
        //DriverObject->MajorFunction[IRP_MJ_CLOSE]           = gfCreateClose;
        DriverObject->MajorFunction[IRP_MJ_READ]            = gfReadWrite;
        DriverObject->MajorFunction[IRP_MJ_WRITE]           = gfReadWrite;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = gfDeviceControl;
        DriverObject->MajorFunction[IRP_MJ_PNP]             = gfDispatchPnp;
        DriverObject->MajorFunction[IRP_MJ_POWER]           = gfDispatchPower;
        DriverObject->DriverExtension->AddDevice            = gfFdoAddDevice;
        //DriverObject->DriverUnload                          = gfUnload;

        RtlInitUnicodeString(&usDosName, DOS_DEVICE_NAME);
        status = IoCreateSymbolicLink(&usDosName, &usNTName);
        if (!NT_SUCCESS(status)) {
            
            DbgPrint("failed to create symbolic link %x\n", status);
            IoDeleteDevice(DriverObject->DeviceObject);
        } else {
            DbgPrint("initialized %ws\n", usNTName.Buffer);
        }
        
        deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;
        
    }
    InitializeListHead(&g_data.HbaList);
    g_data.DrvObj = DriverObject;
    g_data.NumHbas = 0;

    return status;
}

NTSTATUS
gfFdoAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
{
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PDEVICE_EXTENSION       deviceExtension; 

    _asm int 3;
    //
    // Create a filter device object.
    //

    status = IoCreateDevice (DriverObject,
                             sizeof (DEVICE_EXTENSION),
                             NULL,  // No Name
                             PhysicalDeviceObject->Type, //FILE_DEVICE_DISK, //FILE_DEVICE_UNKNOWN,
                             0, //FILE_DEVICE_SECURE_OPEN,
                             FALSE,
                             &deviceObject);

    
    if (!NT_SUCCESS (status)) {
        //
        // Returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        //
        return status;
    }

    KPRINT(("AddDevice PDO (0x%x) FDO (0x%x), Type %d\n", 
                    PhysicalDeviceObject, deviceObject, deviceObject->Type));

    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;

    deviceExtension->Target = IoAttachDeviceToDeviceStack (
                                       deviceObject,
                                       PhysicalDeviceObject);
    //
    // Failure for attachment is an indication of a broken plug & play system.
    //

    if(NULL == deviceExtension->Target) {

        IoDeleteDevice(deviceObject);
        return STATUS_UNSUCCESSFUL;
    }

    deviceObject->Flags |= deviceExtension->Target->Flags & 
                            (DO_BUFFERED_IO | DO_DIRECT_IO | 
                            DO_POWER_PAGABLE  | DO_POWER_INRUSH);
                            
    deviceObject->DeviceType = deviceExtension->Target->DeviceType;
    deviceObject->Characteristics = 
                          deviceExtension->Target->Characteristics;

    deviceExtension->Self = deviceObject;
    deviceExtension->Pdo  = PhysicalDeviceObject;
    deviceExtension->Fdo  = NULL;

    InitializeListHead(&deviceExtension->PdoList);
    InsertHeadList(&g_data.HbaList, &deviceExtension->HbaDeviceEntry);
    g_data.NumHbas++;
    
    INITIALIZE_PNP_STATE(deviceExtension);
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;

}

NTSTATUS
gfPdoAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    OUT PDEVICE_OBJECT *PdoFilterObject
    )
{
    NTSTATUS                status = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PDEVICE_EXTENSION       deviceExtension; 

    _asm int 3;
    //
    // Create a filter device object.
    //

    status = IoCreateDevice (DriverObject,
                             sizeof (DEVICE_EXTENSION),
                             NULL,  // No Name
                             PhysicalDeviceObject->Type, //FILE_DEVICE_DISK, //FILE_DEVICE_UNKNOWN,
                             0, //FILE_DEVICE_SECURE_OPEN,
                             FALSE,
                             &deviceObject);

    
    if (!NT_SUCCESS (status)) {
        //
        // Returning failure here prevents the entire stack from functioning,
        // but most likely the rest of the stack will not be able to create
        // device objects either, so it is still OK.
        //
        return status;
    }

    KPRINT(("AddDevice PDO (0x%x) FDO (0x%x), Type %d\n", 
                    PhysicalDeviceObject, deviceObject, deviceObject->Type));

    *PdoFilterObject = deviceObject;
    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;

    deviceExtension->Target = IoAttachDeviceToDeviceStack (
                                       deviceObject,
                                       PhysicalDeviceObject);
    //
    // Failure for attachment is an indication of a broken plug & play system.
    //

    if(NULL == deviceExtension->Target) {

        IoDeleteDevice(deviceObject);
        return STATUS_UNSUCCESSFUL;
    }

    deviceObject->Flags |= deviceExtension->Target->Flags & 
                            (DO_BUFFERED_IO | DO_DIRECT_IO | 
                            DO_POWER_PAGABLE  | DO_POWER_INRUSH);
                            
    deviceObject->DeviceType = deviceExtension->Target->DeviceType;
    deviceObject->Characteristics = 
                          deviceExtension->Target->Characteristics;

    deviceExtension->Self = deviceObject;
    deviceExtension->Pdo  = PhysicalDeviceObject;
    deviceExtension->Fdo  = NULL;

    InitializeListHead(&deviceExtension->PdoList);
    
    INITIALIZE_PNP_STATE(deviceExtension);
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;

}

NTSTATUS
gfCreateClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    return (gfPass(DeviceObject, Irp));
}

gfCompletionRoutine(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp,
    IN PVOID            Context
    )
/*++
    DeviceObject - Pointer to deviceobject
    Irp          - Pointer to a PnP Irp.
    Context      - Pointer to an event object    
--*/

{
    UNREFERENCED_PARAMETER(DeviceObject);
    
    KeSetEvent((PKEVENT) Context, IO_NO_INCREMENT, FALSE);
    //
    // Allows the caller to use the IRP after it is completed
    //
    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
ForwardIrpSync(
    IN PDEVICE_OBJECT devObj,
    IN PIRP Irp
    )
{
    KEVENT  event;
    NTSTATUS  status;

    KeInitializeEvent(&event, NotificationEvent, FALSE);
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, gfCompletionRoutine,
                            &event, TRUE, TRUE, TRUE);
    status = IoCallDriver(devObj, Irp);
    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = Irp->IoStatus.Status;
    }

    return status;

}

NTSTATUS
gfDeviceControl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS Status;
    PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(Irp);
    PDEVICE_EXTENSION deviceExtension; 
    ULONG code = io_stack->Parameters.DeviceIoControl.IoControlCode;
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    //IoctlPrint(code, DeviceObject->DeviceType);
    return (gfPass(DeviceObject, Irp));
}

NTSTATUS
gfReadWrite (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    DbgPrint("read/write\n");
    return (gfPass(DeviceObject, Irp));
}

NTSTATUS
gfPass (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS Status;
    PDEVICE_EXTENSION  deviceExtension; 
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    IoSkipCurrentIrpStackLocation (Irp);
    return IoCallDriver (deviceExtension->Target, Irp);
}

BOOLEAN 
IsDiskInPdoList (
    PLIST_ENTRY PdoList,
    PDEVICE_OBJECT Pdo
    ) 
/*
 * Checks to see if the Pdo from the DevRel exists in the PdoList.
 *
 */
{
    PPDOLIST PdoCache = NULL;    
    PLIST_ENTRY PdoFList = PdoList;
    if (IsListEmpty(PdoList)) {
        return FALSE;
    }
    
    PdoList = PdoList->Flink;
    while (PdoList && (PdoList != PdoFList)) {
        PdoCache = (PPDOLIST)CONTAINING_RECORD(PdoList, PDOLIST, NextPdo);        
        if (PdoCache->DiskPdo == Pdo) {
            KPRINT(("IsDiskInPdoList:  returning TRUE\n"));
             return TRUE;
        }
        PdoList = PdoList->Flink;            
    }
    KPRINT(("IsDiskInPdoList:  returning FALSE\n"));
    return FALSE;
}

void
RemoveFromPdoList (
    PLIST_ENTRY PdoList,
    PDEVICE_OBJECT Pdo
    ) 
/*
 * Checks to see if the Pdo from the DevRel exists in the PdoList.
 *
 */
{
    PPDOLIST PdoCache = NULL;    
    PLIST_ENTRY PdoFList = PdoList;
    if (IsListEmpty(PdoList)) {
        KPRINT(("Cannot remove from Pdo List, list empty\n"));
        return;
    }
    
    PdoList = PdoList->Flink;
    while (PdoList && (PdoList != PdoFList)) {
        PdoCache = (PPDOLIST)CONTAINING_RECORD(PdoList, PDOLIST, NextPdo);        
        if (PdoCache->DiskPdo == Pdo) {
            KPRINT(("Removing Pdo from List\n"));
            RemoveEntryList(PdoList);
            ExFreePool(PdoCache);
            return;
        }
        PdoList = PdoList->Flink;            
    }
    KPRINT(("Cannot remove from Pdo List, not found\n"));
    return;
}

NTSTATUS
Bus_GetDeviceCapabilities(
    IN  PDEVICE_OBJECT          DeviceObject,
    IN  PDEVICE_CAPABILITIES    DeviceCapabilities
    )
/*++

Routine Description:

    This routine sends the get capabilities irp to the given stack

Arguments:

    DeviceObject        A device object in the stack whose capabilities we want
    DeviceCapabilites   Where to store the answer

Return Value:

    NTSTATUS

--*/
{
    IO_STATUS_BLOCK     ioStatus;
    KEVENT              pnpEvent;
    NTSTATUS            status;
    PDEVICE_OBJECT      targetObject;
    PIO_STACK_LOCATION  irpStack;
    PIRP                pnpIrp;

    PAGED_CODE();

    //
    // Initialize the capabilities that we will send down
    //
    RtlZeroMemory( DeviceCapabilities, sizeof(DEVICE_CAPABILITIES) );
    DeviceCapabilities->Size = sizeof(DEVICE_CAPABILITIES);
    DeviceCapabilities->Version = 1;
    DeviceCapabilities->Address = -1;
    DeviceCapabilities->UINumber = -1;

    //
    // Initialize the event
    //
    KeInitializeEvent( &pnpEvent, NotificationEvent, FALSE );

    targetObject = IoGetAttachedDeviceReference( DeviceObject );

    //
    // Build an Irp
    //
    pnpIrp = IoBuildSynchronousFsdRequest(
        IRP_MJ_PNP,
        targetObject,
        NULL,
        0,
        NULL,
        &pnpEvent,
        &ioStatus
        );
    if (pnpIrp == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetDeviceCapabilitiesExit;

    }

    //
    // Pnp Irps all begin life as STATUS_NOT_SUPPORTED;
    //
    pnpIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;

    //
    // Get the top of stack
    //
    irpStack = IoGetNextIrpStackLocation( pnpIrp );

    //
    // Set the top of stack
    //
    RtlZeroMemory( irpStack, sizeof(IO_STACK_LOCATION ) );
    irpStack->MajorFunction = IRP_MJ_PNP;
    irpStack->MinorFunction = IRP_MN_QUERY_CAPABILITIES;
    irpStack->Parameters.DeviceCapabilities.Capabilities = DeviceCapabilities;

    //
    // Call the driver
    //
    status = IoCallDriver( targetObject, pnpIrp );
    if (status == STATUS_PENDING) {

        //
        // Block until the irp comes back.
        // Important thing to note here is when you allocate 
        // the memory for an event in the stack you must do a  
        // KernelMode wait instead of UserMode to prevent 
        // the stack from getting paged out.
        //

        KeWaitForSingleObject(
            &pnpEvent,
            Executive,
            KernelMode,
            FALSE,
            NULL
            );
        status = ioStatus.Status;

    }

GetDeviceCapabilitiesExit:
    //
    // Done with reference
    //
    ObDereferenceObject( targetObject );

    //
    // Done
    //
    return status;

}


NTSTATUS
QueryPnpState(
    PDEVICE_OBJECT DeviceObject
    )
{
    NTSTATUS Status;
    PIRP Irp;
    KEVENT Event;
    IO_STATUS_BLOCK IoStatus;

    Irp = IoBuildDeviceIoControlRequest(
                                        IRP_MJ_PNP,
                                        DeviceObject,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        FALSE, //IN BOOLEAN  InternalDeviceIoControl,
                                        &Event,
                                        &IoStatus
                                        );
    Status = IoCallDriver(DeviceObject, Irp);

    if (Status == STATUS_PENDING) {
        (VOID) KeWaitForSingleObject(
                   &Event,
                   Suspended,
                   KernelMode,
                   FALSE,
                   NULL
                   );

        Status = Irp->IoStatus.Status;
    } else {

        //
        // Ensure that the proper status value gets picked up.
        //

        IoStatus.Status = Status;
    }

    return(IoStatus.Status);
}

static NTSTATUS
UpdatePdoData (
    PDEVICE_RELATIONS  pDevRel,
    PDEVICE_OBJECT     pFdoObj,
    PDEVICE_RELATIONS *ppNewDevRel
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG i = 0;
    ULONG AddCacheCnt = 0;
    PDEVICE_EXTENSION pFdoExt = pFdoObj->DeviceExtension;
    PDEVICE_EXTENSION pPdoExt = NULL;
    PPDOLIST PdoCache = NULL;    
    PDEVICE_OBJECT pPdoFilterObj = NULL;
    
    KPRINT(("UpdatePdoData:  Num Devs %d\n", pDevRel->Count));
    for (i = 0; i < pDevRel->Count; i++) {
        KPRINT(("Object %x\n", pDevRel->Objects[i]));
        if (!IsDiskInPdoList(&pFdoExt->PdoList, pDevRel->Objects[i])) {
            PdoCache = (PPDOLIST) ExAllocatePoolWithTag(NonPagedPool, sizeof(PDOLIST), POOL_TAG);
            if (PdoCache) {
                PdoCache->DiskPdo = pDevRel->Objects[i];   
                AddCacheCnt++;
                Status = gfPdoAddDevice(g_data.DrvObj,
                                PdoCache->DiskPdo, 
                                &pPdoFilterObj);
                pPdoExt = pPdoFilterObj->DeviceExtension;
                pPdoExt->Fdo = pFdoObj;
                InsertTailList(&pFdoExt->PdoList, &PdoCache->NextPdo);          
                /*Status =*/ QueryPnpState(PdoCache->DiskPdo);
                
            }
        }
    }
    *ppNewDevRel = pDevRel;     /* assume common case is no change */
    return(Status);
}

NTSTATUS
gfDispatchPnp (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PDEVICE_EXTENSION           deviceExtension; 
    PIO_STACK_LOCATION          irpStack;
    NTSTATUS                    status;

    PAGED_CODE();

    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

//    KPRINT("FilterDO %s IRP:0x%x \n", 
//                PnPMinorFunctionString(irpStack->MinorFunction), Irp);
    
    status = STATUS_SUCCESS;
    
    switch (irpStack->MinorFunction) {
    case IRP_MN_QUERY_DEVICE_RELATIONS: {
        DEVICE_RELATION_TYPE Type;
        PDEVICE_RELATIONS    pDevRel = NULL;
        Type = irpStack->Parameters.QueryDeviceRelations.Type;
        if (Type == BusRelations) {
            /* first let underlying device populate the pdo list */
            status = ForwardIrpSync(deviceExtension->Target, Irp);
            if (!NT_SUCCESS(status)) {
                goto qdr_error_exit;
            }
            pDevRel  = (PDEVICE_RELATIONS) Irp->IoStatus.Information;
            status = UpdatePdoData(pDevRel, deviceExtension->Self, &pDevRel);
            if (!NT_SUCCESS(status)) {
                goto qdr_error_exit;
            }
            Irp->IoStatus.Information = (ULONG_PTR)pDevRel;
            goto qdr_error_exit; /* success exit */
        } else {
            status = STATUS_SUCCESS;
        }                                                
        break;
qdr_error_exit:
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
        
    }

    case IRP_MN_START_DEVICE:

        //
        // The device is starting.
        //
        // We cannot touch the device (send it any non pnp irps) until a
        // start device has been passed down to the lower drivers.
        //
        status = ForwardIrpSync(deviceExtension->Target, Irp);
        //
        // We must now complete the IRP, since we stopped it in the
        // completion routine with STATUS_MORE_PROCESSING_REQUIRED.
        //
        SET_NEW_PNP_STATE(deviceExtension, Started);
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
        
    case IRP_MN_REMOVE_DEVICE:
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(deviceExtension->Target, Irp);
        _asm int 3;
        SET_NEW_PNP_STATE(deviceExtension, Deleted);
        if (deviceExtension->Fdo != NULL) {
            PDEVICE_OBJECT Fdo = deviceExtension->Fdo, Pdo = NULL;
            PDEVICE_EXTENSION FdoExt = Fdo->DeviceExtension, PdoExt = deviceExtension;
            RemoveFromPdoList(&FdoExt->PdoList, PdoExt->Pdo);
        }
        IoDetachDevice(deviceExtension->Target); 
        IoDeleteDevice(DeviceObject);
        return status;
        
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
        status = ForwardIrpSync(deviceExtension->Target, Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
        
    case IRP_MN_QUERY_STOP_DEVICE:
        SET_NEW_PNP_STATE(deviceExtension, StopPending);
        status = STATUS_SUCCESS;
        break;
        
    case IRP_MN_CANCEL_STOP_DEVICE:
        //
        // Check to see whether you have received cancel-stop
        // without first receiving a query-stop. This could happen if someone
        // above us fails a query-stop and passes down the subsequent
        // cancel-stop.
        //
        if(StopPending == deviceExtension->DevicePnPState)
        {
            //
            // We did receive a query-stop, so restore.
            //             
            RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
        }
        status = STATUS_SUCCESS; // We must not fail this IRP.
        break;
        
    case IRP_MN_STOP_DEVICE:
        SET_NEW_PNP_STATE(deviceExtension, StopPending);
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_QUERY_REMOVE_DEVICE:
        SET_NEW_PNP_STATE(deviceExtension, RemovePending);
        status = STATUS_SUCCESS;
        break;
       
    case IRP_MN_SURPRISE_REMOVAL:
        SET_NEW_PNP_STATE(deviceExtension, SurpriseRemovePending);
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        //
        // Check to see whether you have received cancel-remove
        // without first receiving a query-remove. This could happen if 
        // someone above us fails a query-remove and passes down the 
        // subsequent cancel-remove.
        //
        if(RemovePending == deviceExtension->DevicePnPState)
        {
            //
            // We did receive a query-remove, so restore.
            //             
            RESTORE_PREVIOUS_PNP_STATE(deviceExtension);
        }
        status = STATUS_SUCCESS; // We must not fail this IRP.
        break;
         
        
    default:
        //
        // If you don't handle any IRP you must leave the
        // status as is.
        //        
        status = Irp->IoStatus.Status;
        break;
    }

    //
    // Pass the IRP down and forget it.
    //
    Irp->IoStatus.Status = status;
    return gfPass(DeviceObject, Irp);
}

NTSTATUS
gfDispatchPower(
    IN PDEVICE_OBJECT    DeviceObject,
    IN PIRP              Irp
    )
{
    PDEVICE_EXTENSION   deviceExtension;
    
    //PAGED_CODE ();
    deviceExtension = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    PoStartNextPowerIrp(Irp);
    IoSkipCurrentIrpStackLocation(Irp);
    return PoCallDriver(deviceExtension->Target, Irp);
}

VOID
gfUnload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++
    Free all the allocated resources in DriverEntry, etc.
--*/
{
    UNICODE_STRING Win32NameString;

    RtlInitUnicodeString(&Win32NameString, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&Win32NameString);

    ASSERT(DriverObject->DeviceObject != NULL);
    IoDeleteDevice(DriverObject->DeviceObject);
    
    DbgPrint("unload\n");
    return;
}

#define IOCTL_TYPE(code) (((ULONG)(code & 0xffff0000)) >> 16)
#define IOCTL_CODE(code) (((ULONG)(code & 0xffff) >>  2) & 0x0fff)
void IoctlPrint(ULONG IoCode, ULONG Device)
{
    ULONG DeviceType = IOCTL_TYPE(IoCode), Cmd = IOCTL_CODE(IoCode);
   
    DbgPrint("MyIoctl(%08x): Code %03x, Device %04x => %04x :  ", IoCode, Cmd, DeviceType, Device);
    switch (IoCode) {
        case IOCTL_DISK_GET_DRIVE_GEOMETRY:
            DbgPrint("IOCTL_DISK_GET_DRIVE_GEOMETRY\n");
            break;
        case IOCTL_DISK_GET_PARTITION_INFO:
            DbgPrint("IOCTL_DISK_GET_PARTITION_INFO\n");
            break;
        case IOCTL_DISK_SET_PARTITION_INFO:
            DbgPrint("IOCTL_DISK_SET_PARTITION_INFO\n");
            break;
        case IOCTL_DISK_GET_DRIVE_LAYOUT:
            DbgPrint("IOCTL_DISK_GET_DRIVE_LAYOUT\n");
            break;
        case IOCTL_DISK_SET_DRIVE_LAYOUT:
            DbgPrint("IOCTL_DISK_SET_DRIVE_LAYOUT\n");
            break;
        case IOCTL_DISK_VERIFY:
            DbgPrint("IOCTL_DISK_VERIFY\n");
            break;
        case IOCTL_DISK_FORMAT_TRACKS:
            DbgPrint("IOCTL_DISK_FORMAT_TRACKS\n");
            break;
        case IOCTL_DISK_REASSIGN_BLOCKS:
            DbgPrint("IOCTL_DISK_REASSIGN_BLOCKS\n");
            break;
        case IOCTL_DISK_PERFORMANCE:
            DbgPrint("IOCTL_DISK_PERFORMANCE\n");
            break;
        case IOCTL_DISK_IS_WRITABLE:
            DbgPrint("IOCTL_DISK_IS_WRITABLE\n");
            break;
        case IOCTL_DISK_LOGGING:
            DbgPrint("IOCTL_DISK_LOGGING\n");
            break;
        case IOCTL_DISK_FORMAT_TRACKS_EX:
            DbgPrint("IOCTL_DISK_FORMAT_TRACKS_EX\n");
            break;
        case IOCTL_DISK_HISTOGRAM_STRUCTURE:
            DbgPrint("IOCTL_DISK_HISTOGRAM_STRUCTURE\n");
            break;
        case IOCTL_DISK_HISTOGRAM_DATA:
            DbgPrint("IOCTL_DISK_HISTOGRAM_DATA\n");
            break;
        case IOCTL_DISK_HISTOGRAM_RESET:
            DbgPrint("IOCTL_DISK_HISTOGRAM_RESET\n");
            break;
        case IOCTL_DISK_REQUEST_STRUCTURE:
            DbgPrint("IOCTL_DISK_REQUEST_STRUCTURE\n");
            break;
        case IOCTL_DISK_REQUEST_DATA:
            DbgPrint("IOCTL_DISK_REQUEST_DATA\n");
            break;
        case IOCTL_DISK_PERFORMANCE_OFF:
            DbgPrint("IOCTL_DISK_PERFORMANCE_OFF\n");
            break;
        case IOCTL_DISK_CONTROLLER_NUMBER:
            DbgPrint("IOCTL_DISK_CONTROLLER_NUMBER\n");
            break;
        case SMART_GET_VERSION:
            DbgPrint("SMART_GET_VERSION\n");
            break;
        case SMART_SEND_DRIVE_COMMAND:
            DbgPrint("SMART_SEND_DRIVE_COMMAND\n");
            break;
        case SMART_RCV_DRIVE_DATA:
            DbgPrint("SMART_RCV_DRIVE_DATA\n");
            break;
        case IOCTL_DISK_GET_PARTITION_INFO_EX:
            DbgPrint("IOCTL_DISK_GET_PARTITION_INFO_EX\n");
            break;
        case IOCTL_DISK_SET_PARTITION_INFO_EX:
            DbgPrint("IOCTL_DISK_SET_PARTITION_INFO_EX\n");
            break;
        case IOCTL_DISK_GET_DRIVE_LAYOUT_EX:
            DbgPrint("IOCTL_DISK_GET_DRIVE_LAYOUT_EX\n");
            break;
        case IOCTL_DISK_SET_DRIVE_LAYOUT_EX:
            DbgPrint("IOCTL_DISK_SET_DRIVE_LAYOUT_EX\n");
            break;
        case IOCTL_DISK_CREATE_DISK:
            DbgPrint("IOCTL_DISK_CREATE_DISK\n");
            break;
        case IOCTL_DISK_GET_LENGTH_INFO:
            DbgPrint("IOCTL_DISK_GET_LENGTH_INFO\n");
            break;
        case IOCTL_DISK_GET_DRIVE_GEOMETRY_EX:
            DbgPrint("IOCTL_DISK_GET_DRIVE_GEOMETRY_EX\n");
            break;
        case IOCTL_DISK_UPDATE_DRIVE_SIZE:
            DbgPrint("IOCTL_DISK_UPDATE_DRIVE_SIZE\n");
            break;
        case IOCTL_DISK_GROW_PARTITION:
            DbgPrint("IOCTL_DISK_GROW_PARTITION\n");
            break;
        case IOCTL_DISK_GET_CACHE_INFORMATION:
            DbgPrint("IOCTL_DISK_GET_CACHE_INFORMATION\n");
            break;
        case IOCTL_DISK_SET_CACHE_INFORMATION:
            DbgPrint("IOCTL_DISK_SET_CACHE_INFORMATION\n");
            break;
#if 0
        case IOCTL_DISK_GET_WRITE_CACHE_STATE:
            DbgPrint("IOCTL_DISK_GET_WRITE_CACHE_STATE\n");
            break;
#endif
        case IOCTL_DISK_DELETE_DRIVE_LAYOUT:
            DbgPrint("IOCTL_DISK_DELETE_DRIVE_LAYOUT\n");
            break;
        case IOCTL_DISK_UPDATE_PROPERTIES:
            DbgPrint("IOCTL_DISK_UPDATE_PROPERTIES\n");
            break;
        case IOCTL_DISK_FORMAT_DRIVE:
            DbgPrint("IOCTL_DISK_FORMAT_DRIVE\n");
            break;
        case IOCTL_DISK_SENSE_DEVICE:
            DbgPrint("IOCTL_DISK_SENSE_DEVICE\n");
            break;
        case IOCTL_DISK_INTERNAL_SET_VERIFY:
            DbgPrint("IOCTL_DISK_INTERNAL_SET_VERIFY\n");
            break;
        case IOCTL_DISK_INTERNAL_CLEAR_VERIFY:
            DbgPrint("IOCTL_DISK_INTERNAL_CLEAR_VERIFY\n");
            break;
        case IOCTL_DISK_INTERNAL_SET_NOTIFY:
            DbgPrint("IOCTL_DISK_INTERNAL_SET_NOTIFY\n");
            break;
        case IOCTL_DISK_CHECK_VERIFY:
            DbgPrint("IOCTL_DISK_CHECK_VERIFY\n");
            break;
        case IOCTL_DISK_MEDIA_REMOVAL:
            DbgPrint("IOCTL_DISK_MEDIA_REMOVAL\n");
            break;
        case IOCTL_DISK_EJECT_MEDIA:
            DbgPrint("IOCTL_DISK_EJECT_MEDIA\n");
            break;
        case IOCTL_DISK_LOAD_MEDIA:
            DbgPrint("IOCTL_DISK_LOAD_MEDIA\n");
            break;
        case IOCTL_DISK_RESERVE:
            DbgPrint("IOCTL_DISK_RESERVE\n");
            break;
        case IOCTL_DISK_RELEASE:
            DbgPrint("IOCTL_DISK_RELEASE\n");
            break;
        case IOCTL_DISK_FIND_NEW_DEVICES:
            DbgPrint("IOCTL_DISK_FIND_NEW_DEVICES\n");
            break;
        case IOCTL_DISK_GET_MEDIA_TYPES:
            DbgPrint("IOCTL_DISK_GET_MEDIA_TYPES\n");
            break;
        case IOCTL_STORAGE_CHECK_VERIFY:
            DbgPrint("IOCTL_STORAGE_CHECK_VERIFY\n");
            break;
        case IOCTL_STORAGE_CHECK_VERIFY2:
            DbgPrint("IOCTL_STORAGE_CHECK_VERIFY2\n");
            break;
        case IOCTL_STORAGE_MEDIA_REMOVAL:
            DbgPrint("IOCTL_STORAGE_MEDIA_REMOVAL\n");
            break;
        case IOCTL_STORAGE_EJECT_MEDIA:
            DbgPrint("IOCTL_STORAGE_EJECT_MEDIA\n");
            break;
        case IOCTL_STORAGE_LOAD_MEDIA:
            DbgPrint("IOCTL_STORAGE_LOAD_MEDIA\n");
            break;
        case IOCTL_STORAGE_LOAD_MEDIA2:
            DbgPrint("IOCTL_STORAGE_LOAD_MEDIA2\n");
            break;
        case IOCTL_STORAGE_RESERVE:
            DbgPrint("IOCTL_STORAGE_RESERVE\n");
            break;
        case IOCTL_STORAGE_RELEASE:
            DbgPrint("IOCTL_STORAGE_RELEASE\n");
            break;
        case IOCTL_STORAGE_FIND_NEW_DEVICES:
            DbgPrint("IOCTL_STORAGE_FIND_NEW_DEVICES\n");
            break;
        case IOCTL_STORAGE_EJECTION_CONTROL:
            DbgPrint("IOCTL_STORAGE_EJECTION_CONTROL\n");
            break;
        case IOCTL_STORAGE_MCN_CONTROL:
            DbgPrint("IOCTL_STORAGE_MCN_CONTROL\n");
            break;
        case IOCTL_STORAGE_GET_MEDIA_TYPES:
            DbgPrint("IOCTL_STORAGE_GET_MEDIA_TYPES\n");
            break;
        case IOCTL_STORAGE_GET_MEDIA_TYPES_EX:
            DbgPrint("IOCTL_STORAGE_GET_MEDIA_TYPES_EX\n");
            break;
        case IOCTL_STORAGE_RESET_BUS:
            DbgPrint("IOCTL_STORAGE_RESET_BUS\n");
            break;
        case IOCTL_STORAGE_RESET_DEVICE:
            DbgPrint("IOCTL_STORAGE_RESET_DEVICE\n");
            break;
        case IOCTL_STORAGE_GET_DEVICE_NUMBER:
            DbgPrint("IOCTL_STORAGE_GET_DEVICE_NUMBER\n");
            break;
        case IOCTL_STORAGE_PREDICT_FAILURE:
            DbgPrint("IOCTL_STORAGE_PREDICT_FAILURE\n");
            break;
        case IOCTL_STORAGE_QUERY_PROPERTY:
            DbgPrint("IOCTL_STORAGE_QUERY_PROPERTY\n");
            break;
        default:
            DbgPrint("Unknown Ioctl 0x%x\n", IoCode);
            break;
    }
}

