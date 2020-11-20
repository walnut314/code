/*++
Module Name:
    modern.c

Abstract:

Environment:
    Kernel mode only.

Project Status:
    1. change devExt to DeviceContext _DEVICE_CONTEXT
    5. check this out: https://github.com/microsoft/Windows-driver-samples/blob/master/serial/VirtualSerial2/queue.c

--*/

#include "modern.h"

//
// The trace message header file must be included in a source file
// before any WPP macro calls and after defining a WPP_CONTROL_GUIDS
// macro. During the compilation, WPP scans the source files for
// TraceEvents() calls and builds a .tmh file which stores a unique
// data GUID for each message, the text resource string for each message,
// and the data types of the variables passed in for each message.
// This file is automatically generated and used during post-processing.
//
#include "modern.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT, DriverEntry )
#pragma alloc_text( PAGE, ModernDeviceAdd)
#pragma alloc_text( PAGE, ModernEvtDriverContextCleanup)
#pragma alloc_text( PAGE, ModernEvtDriverUnload)
//#pragma alloc_text( PAGE, ModernEvtDeviceIoInCallerContext)
#pragma alloc_text( PAGE, ModernEvtDeviceFileCreate)
#pragma alloc_text( PAGE, ModernEvtFileClose)
#pragma alloc_text( PAGE, FileEvtIoRead)
#pragma alloc_text( PAGE, FileEvtIoWrite)
#pragma alloc_text( PAGE, FileEvtIoDeviceControl)
#endif // ALLOC_PRAGMA

MYREQUEST MyRequest;
WDFREQUEST MyWdfRequest;

NTSTATUS
DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
    )
/*++

Routine Description:
    This routine is called by the Operating System to initialize the driver.

    It creates the device object, fills in the dispatch entry points and
    completes the initialization.

Arguments:
    DriverObject - a pointer to the object that represents this device
    driver.

    RegistryPath - a pointer to our Services key in the registry.

Return Value:
    STATUS_SUCCESS if initialized; an error otherwise.

--*/
{
    NTSTATUS                        status;
    WDF_DRIVER_CONFIG               config;
    WDFDRIVER                       hDriver;
    PWDFDEVICE_INIT                 pInit = NULL;
    WDF_OBJECT_ATTRIBUTES           attributes;

    KdPrint(("Driver Frameworks Modern Legacy Driver Example\n"));

    WDF_DRIVER_CONFIG_INIT(
        &config,
        WDF_NO_EVENT_CALLBACK // This is a non-pnp driver.
        );

    // Tell the framework that this is non-pnp driver so that it doesn't
    // set the default AddDevice routine.
    config.DriverInitFlags |= WdfDriverInitNonPnpDriver;

    // Modern driver must explicitly register an unload routine for
    // the driver to be unloaded.
    config.EvtDriverUnload = ModernEvtDriverUnload;

    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = ModernEvtDriverContextCleanup;

    // Create a framework driver object to represent our driver.
    status = WdfDriverCreate(DriverObject,
                            RegistryPath,
                            &attributes,
                            &config,
                            &hDriver);
    if (!NT_SUCCESS(status)) {
        KdPrint (("Modern: WdfDriverCreate failed with status 0x%x\n", status));
        return status;
    }

    // Since we are calling WPP_CLEANUP in the DriverContextCleanup
    // callback we should initialize WPP Tracing after WDFDRIVER
    // object is created to ensure that we cleanup WPP properly
    // if we return failure status from DriverEntry. This
    // eliminates the need to call WPP_CLEANUP in every path
    // of DriverEntry.
    WPP_INIT_TRACING( DriverObject, RegistryPath );

    // On Win2K system,  you will experience some delay in getting trace events
    // due to the way the ETW is activated to accept trace messages.
    KdPrint(("Modern: DriverEntry: tracing enabled\n"));

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT,
                   "Driver Frameworks Modern Legacy Driver Example");

    // In order to create a control device, we first need to allocate a
    // WDFDEVICE_INIT structure and set all properties.
    pInit = WdfControlDeviceInitAllocate(
                            hDriver,
                            &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R
                            );

    if (pInit == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        return status;
    }

#if DBG
    DbgBreakPoint();
#endif

    // Call ModernDeviceAdd to create a deviceobject to represent our
    // software device.
    status = ModernDeviceAdd(hDriver, pInit);

    return status;
}

NTSTATUS
ModernDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Called by the DriverEntry to create a control-device. This call is
    responsible for freeing the memory for DeviceInit.

Arguments:

    DriverObject - a pointer to the object that represents this device
    driver.

    DeviceInit - Pointer to a driver-allocated WDFDEVICE_INIT structure.

Return Value:

    STATUS_SUCCESS if initialized; an error otherwise.

--*/
{
    NTSTATUS               status;
    WDF_OBJECT_ATTRIBUTES  attributes;
    WDF_IO_QUEUE_CONFIG    ioQueueConfig;
    WDF_FILEOBJECT_CONFIG  fileConfig;
    WDFQUEUE               queue;
    WDFDEVICE              controlDevice;
    PCONTROL_DEVICE_EXTENSION devExt = NULL;

    DECLARE_CONST_UNICODE_STRING(ntDeviceName, NTDEVICE_NAME_STRING) ;
    DECLARE_CONST_UNICODE_STRING(symbolicLinkName, SYMBOLIC_NAME_STRING) ;

    UNREFERENCED_PARAMETER( Driver );

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT,
                   "ModernDeviceAdd DeviceInit %p\n", DeviceInit);
    // Set exclusive to TRUE so that no more than one app can talk to the
    // control device at any time.
    WdfDeviceInitSetExclusive(DeviceInit, TRUE);

    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);


    status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfDeviceInitAssignName failed %!STATUS!", status);
        goto End;
    }

    WdfControlDeviceInitSetShutdownNotification(DeviceInit,
                                                ModernShutdown,
                                                WdfDeviceShutdown);

    // Initialize WDF_FILEOBJECT_CONFIG_INIT struct to tell the
    // framework whether you are interested in handling Create, Close and
    // Cleanup requests that gets generated when an application or another
    // kernel component opens an handle to the device. If you don't register
    // the framework default behaviour would be to complete these requests
    // with STATUS_SUCCESS. A driver might be interested in registering these
    // events if it wants to do security validation and also wants to maintain
    // per handle (fileobject) context.
    WDF_FILEOBJECT_CONFIG_INIT(
                        &fileConfig,
                        ModernEvtDeviceFileCreate,
                        ModernEvtFileClose,
                        WDF_NO_EVENT_CALLBACK // not interested in Cleanup
                        );

    WdfDeviceInitSetFileObjectConfig(DeviceInit,
                                       &fileConfig,
                                       WDF_NO_OBJECT_ATTRIBUTES);

    // Specify the size of device context
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes,
                                    CONTROL_DEVICE_EXTENSION);

    status = WdfDeviceCreate(&DeviceInit,
                             &attributes,
                             &controlDevice);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfDeviceCreate failed %!STATUS!", status);
        goto End;
    }

    devExt = ControlGetData(controlDevice);
    devExt->Device = controlDevice;
    
    // Create a symbolic link for the control object so that usermode can open
    // the device.
    status = WdfDeviceCreateSymbolicLink(controlDevice,
                                &symbolicLinkName);

    if (!NT_SUCCESS(status)) {
        // Control device will be deleted automatically by the framework.
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfDeviceCreateSymbolicLink failed %!STATUS!", status);
        goto End;
    }

    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                    WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoRead = FileEvtIoRead;
    ioQueueConfig.EvtIoWrite = FileEvtIoWrite;
    ioQueueConfig.EvtIoDeviceControl = FileEvtIoDeviceControl;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    // Since we are using Zw function set execution level to passive so that
    // framework ensures that our Io callbacks called at only passive-level
    // even if the request came in at DISPATCH_LEVEL from another driver.
    //
    //attributes.ExecutionLevel = WdfExecutionLevelPassive;

    // By default, Static Driver Verifier (SDV) displays a warning if it 
    // doesn't find the EvtIoStop callback on a power-managed queue. 
    // The 'assume' below causes SDV to suppress this warning. If the driver 
    // has not explicitly set PowerManaged to WdfFalse, the framework creates
    // power-managed queues when the device is not a filter driver.  Normally 
    // the EvtIoStop is required for power-managed queues, but for this driver
    // it is not needed b/c the driver doesn't hold on to the requests or 
    // forward them to other drivers. This driver completes the requests 
    // directly in the queue's handlers. If the EvtIoStop callback is not 
    // implemented, the framework waits for all driver-owned requests to be
    // done before moving in the Dx/sleep states or before removing the 
    // device, which is the correct behavior for this type of driver.
    // If the requests were taking an indeterminate amount of time to complete,
    // or if the driver forwarded the requests to a lower driver/another stack,
    // the queue should have an EvtIoStop/EvtIoResume.
    __analysis_assume(ioQueueConfig.EvtIoStop != 0);
    status = WdfIoQueueCreate(controlDevice,
                              &ioQueueConfig,
                              &attributes,
                              &queue // pointer to default queue
                              );
    __analysis_assume(ioQueueConfig.EvtIoStop == 0);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfIoQueueCreate failed %!STATUS!", status);
        goto End;
    }


    // internally managed queue
    WDF_IO_QUEUE_CONFIG_INIT(&ioQueueConfig, WdfIoQueueDispatchManual);
    ioQueueConfig.PowerManaged = WdfFalse;
    status = WdfIoQueueCreate(controlDevice,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              &devExt->MsgQueue);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfIoQueueCreate failed msg queue %!STATUS!", status);
        goto End;
    }


    // Control devices must notify WDF when they are done initializing.   I/O is
    // rejected until this call is made.
    WdfControlFinishInitializing(controlDevice);

End:
    // If the device is created successfully, framework would clear the
    // DeviceInit value. Otherwise device create must have failed so we
    // should free the memory ourself.
    if (DeviceInit != NULL) {
        WdfDeviceInitFree(DeviceInit);
    }

    if (NT_SUCCESS(status)) {
        ASSERT(devExt != NULL);
        if (ENABLE_THEAD) {
            status = ModernCreateDevice(devExt->Device);
        }
    }
    return status;

}

NTSTATUS
ModernCreateDevice(
    IN WDFDEVICE Device
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PCONTROL_DEVICE_EXTENSION devExt;

    devExt = ControlGetData(Device);

    KeInitializeSpinLock(
            &devExt->IrpQueueSpinLock);

    InitializeListHead(
            &devExt->IrpQueueListHead);

    KeInitializeSemaphore(
            &devExt->IrpQueueSemaphore,
            0,
            MAXLONG);

    KeInitializeEvent(
            &devExt->AdapterObjectIsAcquired,
            SynchronizationEvent,
            FALSE);

    KeInitializeEvent(
            &devExt->IrpQueueEventStart,
            SynchronizationEvent,
            FALSE);

    KeInitializeEvent(
            &devExt->IrpQueueEventStop,
            SynchronizationEvent,
            FALSE);

    KeInitializeEvent(
            &devExt->DeviceOperationComplete,
            SynchronizationEvent,
            FALSE);

    devExt->ThreadShouldStop = FALSE;

    status = PsCreateSystemThread(
            &devExt->ThreadHandle,
            (ACCESS_MASK) 0,
            NULL,
            (HANDLE) 0,
            NULL,
            ModernThreadMain,
            devExt);

    if (!NT_SUCCESS(status)) {
        // TODO
        // clean up symlink and device object
        return status;
    }

    ObReferenceObjectByHandle(
            devExt->ThreadHandle,
            THREAD_ALL_ACCESS,
            NULL,
            KernelMode,
            &devExt->ThreadObject,
            NULL);

    ZwClose(devExt->ThreadHandle);

    return status;
}

VOID
ModernEvtDriverContextCleanup(
    IN WDFOBJECT Driver
    )
/*++
Routine Description:

   Called when the driver object is deleted during driver unload.
   You can free all the resources created in DriverEntry that are
   not automatically freed by the framework.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

Return Value:

    NTSTATUS

--*/
{
    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT,
                        "Entered ModernEvtDriverContextCleanup\n");

    PAGED_CODE();
    DbgBreakPoint();

    // No need to free the controldevice object explicitly because it will
    // be deleted when the Driver object is deleted due to the default parent
    // child relationship between Driver and ControlDevice.
    WPP_CLEANUP( WdfDriverWdmGetDriverObject( (WDFDRIVER)Driver ) );
}



VOID
ModernEvtDeviceFileCreate (
    IN WDFDEVICE     Device,
    IN WDFREQUEST    Request,
    IN WDFFILEOBJECT FileObject
    )
/*++

Routine Description:

    The framework calls a driver's EvtDeviceFileCreate callback
    when it receives an IRP_MJ_CREATE request.
    The system sends this request when a user application opens the
    device to perform an I/O operation, such as reading or writing a file.
    This callback is called synchronously, in the context of the thread
    that created the IRP_MJ_CREATE request.

Arguments:

    Device - Handle to a framework device object.
    FileObject - Pointer to fileobject that represents the open handle.
    CreateParams - Parameters of IO_STACK_LOCATION for create

Return Value:

   NT status code

--*/
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER( Device );
    UNREFERENCED_PARAMETER( Request );
    UNREFERENCED_PARAMETER( FileObject );

    PAGED_CODE ();
    DbgBreakPoint();
    WdfRequestComplete(Request, status);

    return;
}


VOID
ModernEvtFileClose (
    IN WDFFILEOBJECT    FileObject
    )

/*++

Routine Description:

   EvtFileClose is called when all the handles represented by the FileObject
   is closed and all the references to FileObject is removed. This callback
   may get called in an arbitrary thread context instead of the thread that
   called CloseHandle. If you want to delete any per FileObject context that
   must be done in the context of the user thread that made the Create call,
   you should do that in the EvtDeviceCleanp callback.

Arguments:

    FileObject - Pointer to fileobject that represents the open handle.

Return Value:

   VOID

--*/
{
    UNREFERENCED_PARAMETER( FileObject );

    PAGED_CODE ();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "ModernEvtFileClose\n");
    return;
}


VOID
FileEvtIoRead(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t            Length
    )
/*++

Routine Description:

    This event is called when the framework receives IRP_MJ_READ requests.
    We will just read the file.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
            I/O request.
    Request - Handle to a framework request object.

    Length  - number of bytes to be read.
                   Queue is by default configured to fail zero length read & write requests.

Return Value:

  None.

--*/
{
    NTSTATUS                   status = STATUS_SUCCESS;
    PVOID                       outBuf;
    IO_STATUS_BLOCK             ioStatus;
    PCONTROL_DEVICE_EXTENSION   devExt;
    FILE_POSITION_INFORMATION   position;
    ULONG_PTR                   bytesRead = 0;
    size_t  bufLength;

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_RW, "FileEvtIoRead: Request: 0x%p, Queue: 0x%p\n",
                     Request, Queue);

    PAGED_CODE ();
    DbgBreakPoint();

    // Get the request buffer. Since the device is set to do buffered
    // I/O, this function will retrieve Irp->AssociatedIrp.SystemBuffer.
    status = WdfRequestRetrieveOutputBuffer(Request, 0, &outBuf, &bufLength);
    if(!NT_SUCCESS(status)) {
        WdfRequestComplete(Request, status);
        return;

    }

    devExt = ControlGetData(WdfIoQueueGetDevice(Queue));

    if(devExt->FileHandle) {
        // Set the file position to the beginning of the file.
        position.CurrentByteOffset.QuadPart = 0;
        status = ZwSetInformationFile(devExt->FileHandle,
                             &ioStatus,
                             &position,
                             sizeof(FILE_POSITION_INFORMATION),
                             FilePositionInformation);
        if (NT_SUCCESS(status)) {

            status = ZwReadFile (devExt->FileHandle,
                                NULL,//   Event,
                                NULL,// PIO_APC_ROUTINE  ApcRoutine
                                NULL,// PVOID  ApcContext
                                &ioStatus,
                                outBuf,
                                (ULONG)Length,
                                0, // ByteOffset
                                NULL // Key
                                );

            if (!NT_SUCCESS(status)) {

                TraceEvents(TRACE_LEVEL_ERROR, DBG_RW,
                               "ZwReadFile failed with status 0x%x",
                               status);
            }

            status = ioStatus.Status;
            bytesRead = ioStatus.Information;
        }
    }

    WdfRequestCompleteWithInformation(Request, status, bytesRead);
}

VOID
FileEvtIoWrite(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t            Length
    )
/*++

Routine Description:

    This event is called when the framework receives IRP_MJ_WRITE requests.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
            I/O request.
    Request - Handle to a framework request object.

    Length  - number of bytes to be written.
                   Queue is by default configured to fail zero length read & write requests.


Return Value:

   None
--*/
{
    NTSTATUS                   status = STATUS_SUCCESS;
    PVOID                       inBuf;
    IO_STATUS_BLOCK             ioStatus;
    PCONTROL_DEVICE_EXTENSION   devExt;
    FILE_POSITION_INFORMATION   position;
    ULONG_PTR                   bytesWritten = 0;
    size_t      bufLength;


    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_RW, "FileEvtIoWrite: Request: 0x%p, Queue: 0x%p\n",
                     Request, Queue);
    PAGED_CODE ();
    DbgBreakPoint();

    // Get the request buffer. Since the device is set to do buffered
    // I/O, this function will retrieve Irp->AssociatedIrp.SystemBuffer.
    status = WdfRequestRetrieveInputBuffer(Request, 0, &inBuf, &bufLength);
    if(!NT_SUCCESS(status)) {
        WdfRequestComplete(Request, status);
        return;

    }

    devExt = ControlGetData(WdfIoQueueGetDevice(Queue));

    if(devExt->FileHandle) {
        // Set the file position to the beginning of the file.
        position.CurrentByteOffset.QuadPart = 0;

        status = ZwSetInformationFile(devExt->FileHandle,
                             &ioStatus,
                             &position,
                             sizeof(FILE_POSITION_INFORMATION),
                             FilePositionInformation);
        if (NT_SUCCESS(status))
        {

            status = ZwWriteFile(devExt->FileHandle,
                                NULL,//   Event,
                                NULL,// PIO_APC_ROUTINE  ApcRoutine
                                NULL,// PVOID  ApcContext
                                &ioStatus,
                                inBuf,
                                (ULONG)Length,
                                0, // ByteOffset
                                NULL // Key
                                );
            if (!NT_SUCCESS(status))
            {
                TraceEvents(TRACE_LEVEL_ERROR, DBG_RW,
                               "ZwWriteFile failed with status 0x%x",
                               status);
            }

            status = ioStatus.Status;
            bytesWritten =  ioStatus.Information;
        }
    }

    WdfRequestCompleteWithInformation(Request, status, bytesWritten);
}

VOID
FileEvtIoDeviceControl(
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
    NTSTATUS            status = STATUS_SUCCESS;// Assume success
    //PCHAR               inBuf = NULL, outBuf = NULL; // pointer to Input and output buffer
    //PCHAR               data = "this String is from Device Driver !!!";
    //ULONG               datalen = (ULONG) strlen(data)+1;//Length of data including null
    //PCHAR               buffer = NULL;
    //PREQUEST_CONTEXT    reqContext = NULL;
    //size_t              bufSize;
    PCONTROL_DEVICE_EXTENSION DeviceContext;
    WDFDEVICE           Device;
    //PIRP                irp;
    //BOOLEAN             completeRequest = TRUE;
    BOOLEAN RequestPending = FALSE;
    size_t BytesReturned = 0;
        
    Device = WdfIoQueueGetDevice(Queue);
    DeviceContext = ControlGetData(Device);

    PAGED_CODE();
    DbgBreakPoint();

    if(!OutputBufferLength || !InputBufferLength)
    {
        WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
        return;
    }

    // Determine which I/O control code was specified.
    switch (IoControlCode)
    {
    case IOCTL_MODERN_START_THREAD:
        DeviceContext->ThreadShouldStop = FALSE;
        //KeSetEvent(
        //        &DeviceContext->IrpQueueEventStart,
        //        IO_NO_INCREMENT,
        //        FALSE);
        break;

    case IOCTL_MODERN_STOP_THREAD:
        DeviceContext->ThreadShouldStop = TRUE;
        //KeSetEvent(
        //        &DeviceContext->IrpQueueEventStop,
        //        IO_NO_INCREMENT,
        //        FALSE);
        break;
            
    case IOCTL_MODERN_QUEUE_REQUEST:
#if 0
        irp = WdfRequestWdmGetIrp(Request);
        IoMarkIrpPending(irp);
        MyWdfRequest = Request;
        ExInterlockedInsertTailList(
                &DeviceContext->IrpQueueListHead,
                &irp->Tail.Overlay.ListEntry,
                &DeviceContext->IrpQueueSpinLock);
        KeReleaseSemaphore(
                &DeviceContext->IrpQueueSemaphore,
                0, // no priority boost
                1, // increment by 1 entry
                FALSE); // no wait after this call
        status = STATUS_PENDING;
#endif
        status = WdfRequestForwardToIoQueue(Request,
                                            DeviceContext->MsgQueue);
        
        if (NT_SUCCESS(status))
        {
            RequestPending = TRUE;
        }
        KeReleaseSemaphore(
                &DeviceContext->IrpQueueSemaphore,
                0, // no priority boost
                1, // increment by 1 entry
                FALSE); // no wait after this call
#if 0
        MyRequest.Request = Request;
        ExInterlockedInsertTailList(
                &DeviceContext->IrpQueueListHead,
                &MyRequest.Entry,
                &DeviceContext->IrpQueueSpinLock);

#endif
        break;

    default:
        // The specified I/O control code is unrecognized by this driver.
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "ERROR: unrecognized IOCTL %x\n", IoControlCode);
        break;
    }

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "Completing Request %p with status %X",
                   Request, status );

    //if (completeRequest == TRUE) {
    //    WdfRequestComplete( Request, status);
    //}
    if (RequestPending == FALSE)
    {
        WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, BytesReturned);
    }

}

VOID
ModernShutdown(
    WDFDEVICE Device
    )
/*++

Routine Description:
    Callback invoked when the machine is shutting down.  If you register for
    a last chance shutdown notification you cannot do the following:
    o Call any pageable routines
    o Access pageable memory
    o Perform any file I/O operations

    If you register for a normal shutdown notification, all of these are
    available to you.

    This function implementation does nothing, but if you had any outstanding
    file handles open, this is where you would close them.

Arguments:
    Device - The device which registered the notification during init

Return Value:
    None

  --*/

{
    UNREFERENCED_PARAMETER(Device);
    DbgBreakPoint();

    return;
}


VOID
ModernEvtDriverUnload(
    IN WDFDRIVER Driver
    )
/*++
Routine Description:

   Called by the I/O subsystem just before unloading the driver.
   You can free the resources created in the DriverEntry either
   in this routine or in the EvtDriverContextCleanup callback.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

Return Value:

    NTSTATUS

--*/
{
    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();
    DbgBreakPoint();

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "Entered ModernDriverUnload\n");

    return;
}

VOID
PrintChars(
    _In_reads_(CountChars) PCHAR BufferAddress,
    _In_ size_t CountChars
    )
{
    if (CountChars) {

        while (CountChars--) {

            if (*BufferAddress > 31
                 && *BufferAddress != 127) {

                KdPrint (( "%c", *BufferAddress) );

            } else {

                KdPrint(( ".") );

            }
            BufferAddress++;
        }
        KdPrint (("\n"));
    }
    return;
}

VOID
ModernThreadMain(
    IN PVOID Context
    )
{
    PCONTROL_DEVICE_EXTENSION DeviceContext = Context;
    //PLIST_ENTRY ListEntry;
    //PIRP Irp;
    //CCHAR PriorityBoost;
    //PVOID waitObjects[3];
    //NTSTATUS waitStatus;
    //MYREQUEST *pMyRequest;
    WDFREQUEST Request;
    NTSTATUS status;
    size_t BytesReturned = 0;
    PSWITCH_STATE SwitchState = NULL;

    KeSetPriorityThread(
            KeGetCurrentThread(),
            LOW_REALTIME_PRIORITY);

    // enter main Irp processing loop
    //waitObjects[0] = (PVOID) &DeviceContext->IrpQueueSemaphore;    // STATUS_WAIT_0
    //waitObjects[1] = (PVOID) &DeviceContext->IrpQueueEventStart;   // STATUS_WAIT_1
    //waitObjects[2] = (PVOID) &DeviceContext->IrpQueueEventStop;    // STATUS_WAIT_2
#if 1
    // 
    while (TRUE) {
        KeWaitForSingleObject(
            &DeviceContext->IrpQueueSemaphore,
            Executive,
            KernelMode,
            FALSE,
            NULL);

        if (DeviceContext->ThreadShouldStop) {
            DbgBreakPoint();
            PsTerminateSystemThread(STATUS_SUCCESS);
        }

        status = WdfIoQueueRetrieveNextRequest(DeviceContext->MsgQueue,
                                               &Request);

        if (NT_SUCCESS(status))
        {
            status = WdfRequestRetrieveOutputBuffer(Request,
                                                    sizeof(SWITCH_STATE),
                                                    &SwitchState,
                                                    NULL);
            if (NT_SUCCESS(status))
            {
                BytesReturned = sizeof(SWITCH_STATE);
                SwitchState->State ^= 1;
                WdfRequestCompleteWithInformation(Request, status, BytesReturned);
            }
        }
    }

#if 0
        ListEntry = ExInterlockedRemoveHeadList(
                &DeviceContext->IrpQueueListHead,
                &DeviceContext->IrpQueueSpinLock);

        Irp = CONTAINING_RECORD(
                ListEntry,
                IRP,
                Tail.Overlay.ListEntry);

        PriorityBoost = IO_NO_INCREMENT;

        //IoCompleteRequest(Irp, PriorityBoost);
        Request = MyWdfRequest;
        WdfRequestComplete( Request, STATUS_SUCCESS);
#endif
#endif
#if 0
    waitStatus = STATUS_WAIT_0;
    while (STATUS_WAIT_1 != waitStatus)
    {
        waitStatus = KeWaitForMultipleObjects(ARRAYSIZE(waitObjects),
                                              waitObjects,
                                              WaitAny,
                                              Executive,DeviceContext
                                              KernelMode,
                                              FALSE,
                                              NULL,
                                              NULL);

#if 0
        if (devExt->ThreadShouldStop) {
            DbgBreakPoint();
            PsTerminateSystemThread(STATUS_SUCCESS);
        }
#endif
        switch (waitStatus)
        {
            case STATUS_WAIT_0:
            {
                ListEntry = ExInterlockedRemoveHeadList(
                        &devExt->IrpQueueListHead,
                        &devExt->IrpQueueSpinLock);
        
                pMyRequest = CONTAINING_RECORD(
                            ListEntry,
                            MYREQUEST,
                            Entry);

                Request = pMyRequest->Request;

                WdfRequestComplete( Request, STATUS_SUCCESS);
                break;
            }
            case STATUS_WAIT_1:
            {
                break;
            }
            case STATUS_WAIT_2:
            {
                break;
            }
            default:
            {
                break;
            }
        }

    }
#endif
}

