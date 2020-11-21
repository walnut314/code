/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    driver.c

Abstract:

    This driver demonstrates use of a default I/O Queue, its
    request start events, cancellation event, and a synchronized DPC.

    To demonstrate asynchronous operation, the I/O requests are not completed
    immediately, but stored in the drivers private data structure, and a timer
    will complete it next time the Timer callback runs.

    During the time the request is waiting for the timer callback to run, it is
    made cancellable by the call WdfRequestMarkCancelable. This
    allows the test program to cancel the request and exit instantly.

    This rather complicated set of events is designed to demonstrate
    the driver frameworks synchronization of access to a device driver
    data structure, and a pointer which can be a proxy for device hardware
    registers or resources.

    This common data structure, or resource is accessed by new request
    events arriving, the Timer callback that completes it, and cancel processing.

    Notice the lack of specific lock/unlock operations.

    Even though this example utilizes a serial queue, a parallel queue
    would not need any additional explicit synchronization, just a
    strategy for managing multiple requests outstanding.

    HKLM\Software\Microsoft\WindowsNT\CurrentVersion\WUDF\Services\UModern
        ImagePath REGSZ ".\UModern.dll"
        WdfMajorVersion DWORD 2
        WdfMinorVersion DWORD 15

--*/

#include "umodern.h"

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    WDF_DRIVER_CONFIG_INIT(&config,
                        EchoEvtDeviceAdd
                        );

    status = WdfDriverCreate(DriverObject,
                            RegistryPath,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &config,
                            WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Error: WdfDriverCreate failed 0x%x\n", status));
        return status;
    }

#if DBG
    EchoPrintDriverVersion();
#endif

    return status;
}

NTSTATUS
EchoEvtDeviceAdd(
    IN WDFDRIVER       Driver,
    IN PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    KdPrint(("Enter  EchoEvtDeviceAdd\n"));

    status = EchoDeviceCreate(DeviceInit);

    return status;
}

NTSTATUS
EchoPrintDriverVersion(
    )
/*++
Routine Description:

   This routine shows how to retrieve framework version string and
   also how to find out to which version of framework library the
   client driver is bound to.

Arguments:

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;
    WDFSTRING string;
    UNICODE_STRING us;
    WDF_DRIVER_VERSION_AVAILABLE_PARAMS ver;

    //
    // 1) Retreive version string and print that in the debugger.
    //
    status = WdfStringCreate(NULL, WDF_NO_OBJECT_ATTRIBUTES, &string);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Error: WdfStringCreate failed 0x%x\n", status));
        return status;
    }

    status = WdfDriverRetrieveVersionString(WdfGetDriver(), string);
    if (!NT_SUCCESS(status)) {
        //
        // No need to worry about delete the string object because
        // by default it's parented to the driver and it will be
        // deleted when the driverobject is deleted when the DriverEntry
        // returns a failure status.
        //
        KdPrint(("Error: WdfDriverRetrieveVersionString failed 0x%x\n", status));
        return status;
    }

    WdfStringGetUnicodeString(string, &us);
    KdPrint(("Echo Sample %wZ\n", &us));

    WdfObjectDelete(string);
    string = NULL; // To avoid referencing a deleted object.

    //
    // 2) Find out to which version of framework this driver is bound to.
    //
    WDF_DRIVER_VERSION_AVAILABLE_PARAMS_INIT(&ver, 1, 0);
    if (WdfDriverIsVersionAvailable(WdfGetDriver(), &ver) == TRUE) {
        KdPrint(("Yes, framework version is 1.0\n"));
    }else {
        KdPrint(("No, framework verison is not 1.0\n"));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
EchoDeviceCreate(
    PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDF_PNPPOWER_EVENT_CALLBACKS    pnpPowerCallbacks;
    WDFDEVICE device;
    NTSTATUS status;

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    //
    // Register pnp/power callbacks so that we can start and stop the timer as the device
    // gets started and stopped.
    //
    pnpPowerCallbacks.EvtDeviceSelfManagedIoInit    = EchoEvtDeviceSelfManagedIoStart;
    pnpPowerCallbacks.EvtDeviceSelfManagedIoSuspend = EchoEvtDeviceSelfManagedIoSuspend;

    #pragma prefast(suppress: 28024, "Function used for both Init and Restart Callbacks")
    pnpPowerCallbacks.EvtDeviceSelfManagedIoRestart = EchoEvtDeviceSelfManagedIoStart;

    //
    // Register the PnP and power callbacks. Power policy related callbacks will be registered
    // later in SotwareInit.
    //
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
        //
        // Get the device context and initialize it. WdfObjectGet_DEVICE_CONTEXT is an
        // inline function generated by WDF_DECLARE_CONTEXT_TYPE macro in the
        // device.h header file. This function will do the type checking and return
        // the device context. If you pass a wrong object  handle
        // it will return NULL and assert if run under framework verifier mode.
        //
        deviceContext = WdfObjectGet_DEVICE_CONTEXT(device);
        deviceContext->PrivateDeviceData = 0;

        //
        // Create a device interface so that application can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_UMODERN,
            NULL // ReferenceString
            );

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = EchoQueueInitialize(device);
        }
    }

    return status;
}


NTSTATUS
EchoEvtDeviceSelfManagedIoStart(
    IN  WDFDEVICE Device
    )
/*++

Routine Description:

    This event is called by the Framework when the device is started
    or restarted after a suspend operation.

    This function is not marked pageable because this function is in the
    device power up path. When a function is marked pagable and the code
    section is paged out, it will generate a page fault which could impact
    the fast resume behavior because the client driver will have to wait
    until the system drivers can service this page fault.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    NTSTATUS - Failures will result in the device stack being torn down.

--*/
{
    PQUEUE_CONTEXT queueContext = QueueGetContext(WdfDeviceGetDefaultQueue(Device));
    LARGE_INTEGER DueTime;

    KdPrint(("--> EchoEvtDeviceSelfManagedIoInit\n"));

    //
    // Restart the queue and the periodic timer. We stopped them before going
    // into low power state.
    //
    WdfIoQueueStart(WdfDeviceGetDefaultQueue(Device));

    DueTime.QuadPart = WDF_REL_TIMEOUT_IN_MS(100);

    WdfTimerStart(queueContext->Timer,  DueTime.QuadPart);

    KdPrint(( "<-- EchoEvtDeviceSelfManagedIoInit\n"));

    return STATUS_SUCCESS;
}

NTSTATUS
EchoEvtDeviceSelfManagedIoSuspend(
    IN  WDFDEVICE Device
    )
/*++

Routine Description:

    This event is called by the Framework when the device is stopped
    for resource rebalance or suspended when the system is entering
    Sx state.


Arguments:

    Device - Handle to a framework device object.

Return Value:

    NTSTATUS - The driver is not allowed to fail this function.  If it does, the
    device stack will be torn down.

--*/
{
    PQUEUE_CONTEXT queueContext = QueueGetContext(WdfDeviceGetDefaultQueue(Device));

    PAGED_CODE();

    KdPrint(("--> EchoEvtDeviceSelfManagedIoSuspend\n"));

    //
    // Before we stop the timer we should make sure there are no outstanding
    // i/o. We need to do that because framework cannot suspend the device
    // if there are requests owned by the driver. There are two ways to solve
    // this issue: 1) We can wait for the outstanding I/O to be complete by the
    // periodic timer 2) Register EvtIoStop callback on the queue and acknowledge
    // the request to inform the framework that it's okay to suspend the device
    // with outstanding I/O. In this sample we will use the 1st approach
    // because it's pretty easy to do. We will restart the queue when the
    // device is restarted.
    //
    WdfIoQueueStopSynchronously(WdfDeviceGetDefaultQueue(Device));

    //
    // Stop the watchdog timer and wait for DPC to run to completion if it's already fired.
    //
    WdfTimerStop(queueContext->Timer, TRUE);

    KdPrint(( "<-- EchoEvtDeviceSelfManagedIoSuspend\n"));

    return STATUS_SUCCESS;
}


NTSTATUS
EchoQueueInitialize(
    WDFDEVICE Device
    )
/*++

Routine Description:


     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for serial request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

     This memory may be used by the driver automatically synchronized
     by the Queue's presentation lock.

     The lifetime of this memory is tied to the lifetime of the I/O
     Queue object, and we register an optional destructor callback
     to release any private allocations, and/or resources.


Arguments:

    Device - Handle to a framework device object.

Return Value:

    NTSTATUS

--*/
{
    WDFQUEUE queue;
    NTSTATUS status;
    PQUEUE_CONTEXT queueContext;
    WDF_IO_QUEUE_CONFIG    queueConfig;
    WDF_OBJECT_ATTRIBUTES  queueAttributes;

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchSequential
        );

    queueConfig.EvtIoRead   = EchoEvtIoRead;
    queueConfig.EvtIoWrite  = EchoEvtIoWrite;

    //
    // Fill in a callback for destroy, and our QUEUE_CONTEXT size
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_CONTEXT);

    //
    // Set synchronization scope on queue and have the timer to use queue as
    // the parent object so that queue and timer callbacks are synchronized
    // with the same lock.
    //
    queueAttributes.SynchronizationScope = WdfSynchronizationScopeQueue;

    queueAttributes.EvtDestroyCallback = EchoEvtIoQueueContextDestroy;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 &queueAttributes,
                 &queue
                 );

    if( !NT_SUCCESS(status) ) {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n",status));
        return status;
    }

    // Get our Driver Context memory from the returned Queue handle
    queueContext = QueueGetContext(queue);

    queueContext->WriteMemory = NULL;
    queueContext->Timer = NULL;

    queueContext->CurrentRequest = NULL;
    queueContext->CurrentStatus = STATUS_INVALID_DEVICE_REQUEST;

    //
    // Create the Queue timer
    //
    status = EchoTimerCreate(&queueContext->Timer, queue);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Error creating timer 0x%x\n",status));
        return status;
    }

    return status;
}


NTSTATUS
EchoTimerCreate(
    IN WDFTIMER*       Timer,
    IN WDFQUEUE        Queue
    )
/*++

Routine Description:

    Subroutine to create timer. By associating the timerobject with
    the queue, we are basically telling the framework to serialize the queue
    callbacks with the timer callback. By doing so, we don't have to worry
    about protecting queue-context structure from multiple threads accessing
    it simultaneously.

Arguments:


Return Value:

    NTSTATUS

--*/
{
    NTSTATUS Status;
    WDF_TIMER_CONFIG       timerConfig;
    WDF_OBJECT_ATTRIBUTES  timerAttributes;

    //
    // Create a non-periodic timer since WDF does not allow periodic timer
    // at passive level, which is the level UMDF callbacks are invoked at.
    // The workaround is to always restart the timer in the timer callback.
    //
    // WDF_TIMER_CONFIG_INIT sets AutomaticSerialization to TRUE by default.
    //
    WDF_TIMER_CONFIG_INIT(&timerConfig, EchoEvtTimerFunc);

    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = Queue; // Synchronize with the I/O Queue
    timerAttributes.ExecutionLevel = WdfExecutionLevelPassive;

    Status = WdfTimerCreate(&timerConfig,
                             &timerAttributes,
                             Timer     // Output handle
                             );

    return Status;
}



VOID
EchoEvtIoQueueContextDestroy(
    WDFOBJECT Object
)
/*++

Routine Description:

    This is called when the Queue that our driver context memory
    is associated with is destroyed.

Arguments:

    Context - Context that's being freed.

Return Value:

    VOID

--*/
{
    PQUEUE_CONTEXT queueContext = QueueGetContext(Object);

    //
    // Release any resources pointed to in the queue context.
    //
    // The body of the queue context will be released after
    // this callback handler returns
    //

    //
    // If Queue context has an I/O buffer, release it
    //
    if( queueContext->WriteMemory != NULL ) {
        WdfObjectDelete(queueContext->WriteMemory);
        queueContext->WriteMemory = NULL;
    }

    return;
}


VOID
EchoEvtRequestCancel(
    IN WDFREQUEST Request
    )
/*++

Routine Description:


    Called when an I/O request is cancelled after the driver has marked
    the request cancellable. This callback is automatically synchronized
    with the I/O callbacks since we have chosen to use frameworks Device
    level locking.

Arguments:

    Request - Request being cancelled.

Return Value:

    VOID

--*/
{
    PQUEUE_CONTEXT queueContext = QueueGetContext(WdfRequestGetIoQueue(Request));

    KdPrint(("EchoEvtRequestCancel called on Request 0x%p\n",  Request));

    //
    // The following is race free by the callside or DPC side
    // synchronizing completion by calling
    // WdfRequestMarkCancelable(Queue, Request, FALSE) before
    // completion and not calling WdfRequestComplete if the
    // return status == STATUS_CANCELLED.
    //
    WdfRequestCompleteWithInformation(Request, STATUS_CANCELLED, 0L);

    //
    // This book keeping is synchronized by the common
    // Queue presentation lock
    //
    ASSERT(queueContext->CurrentRequest == Request);
    queueContext->CurrentRequest = NULL;

    return;
}

VOID
EchoEvtIoRead(
    IN WDFQUEUE   Queue,
    IN WDFREQUEST Request,
    IN size_t      Length
    )
/*++

Routine Description:

    This event is called when the framework receives IRP_MJ_READ request.
    It will copy the content from the queue-context buffer to the request buffer.
    If the driver hasn't received any write request earlier, the read returns zero.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    Length  - number of bytes to be read.
              The default property of the queue is to not dispatch
              zero lenght read & write requests to the driver and
              complete is with status success. So we will never get
              a zero length request.

Return Value:

    VOID

--*/
{
    NTSTATUS Status;
    PQUEUE_CONTEXT queueContext = QueueGetContext(Queue);
    WDFMEMORY memory;
    size_t writeMemoryLength;

    _Analysis_assume_(Length > 0);

    KdPrint(("EchoEvtIoRead Called! Queue 0x%p, Request 0x%p Length %d\n",
             Queue,Request,Length));
    //
    // No data to read
    //
    if( (queueContext->WriteMemory == NULL)  ) {
        WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, (ULONG_PTR)0L);
        return;
    }

    //
    // Read what we have
    //
    WdfMemoryGetBuffer(queueContext->WriteMemory, &writeMemoryLength);
    _Analysis_assume_(writeMemoryLength > 0);

    if( writeMemoryLength < Length ) {
        Length = writeMemoryLength;
    }

    //
    // Get the request memory
    //
    Status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if( !NT_SUCCESS(Status) ) {
        KdPrint(("EchoEvtIoRead Could not get request memory buffer 0x%x\n", Status));
        WdfVerifierDbgBreakPoint();
        WdfRequestCompleteWithInformation(Request, Status, 0L);
        return;
    }

    // Copy the memory out
    Status = WdfMemoryCopyFromBuffer( memory, // destination
                             0,      // offset into the destination memory
                             WdfMemoryGetBuffer(queueContext->WriteMemory, NULL),
                             Length );
    if( !NT_SUCCESS(Status) ) {
        KdPrint(("EchoEvtIoRead: WdfMemoryCopyFromBuffer failed 0x%x\n", Status));
        WdfRequestComplete(Request, Status);
        return;
    }

    // Set transfer information
    WdfRequestSetInformation(Request, (ULONG_PTR)Length);

    // Mark the request is cancelable
    WdfRequestMarkCancelable(Request, EchoEvtRequestCancel);


    // Defer the completion to another thread from the timer dpc
    queueContext->CurrentRequest = Request;
    queueContext->CurrentStatus  = Status;

    return;
}

VOID
EchoEvtIoWrite(
    IN WDFQUEUE   Queue,
    IN WDFREQUEST Request,
    IN size_t     Length
    )
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_WRITE request.
    This routine allocates memory buffer, copies the data from the request to it,
    and stores the buffer pointer in the queue-context with the length variable
    representing the buffers length. The actual completion of the request
    is defered to the periodic timer dpc.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    Length  - number of bytes to be read.
              The default property of the queue is to not dispatch
              zero lenght read & write requests to the driver and
              complete is with status success. So we will never get
              a zero length request.

Return Value:

    VOID

--*/
{
    NTSTATUS Status;
    WDFMEMORY memory;
    PQUEUE_CONTEXT queueContext = QueueGetContext(Queue);
    PVOID writeBuffer = NULL;

    _Analysis_assume_(Length > 0);

    KdPrint(("EchoEvtIoWrite Called! Queue 0x%p, Request 0x%p Length %d\n",
             Queue,Request,Length));

    if( Length > MAX_WRITE_LENGTH ) {
        KdPrint(("EchoEvtIoWrite Buffer Length to big %d, Max is %d\n",
                 Length,MAX_WRITE_LENGTH));
        WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_OVERFLOW, 0L);
        return;
    }

    // Get the memory buffer
    Status = WdfRequestRetrieveInputMemory(Request, &memory);
    if( !NT_SUCCESS(Status) ) {
        KdPrint(("EchoEvtIoWrite Could not get request memory buffer 0x%x\n",
                 Status));
        WdfVerifierDbgBreakPoint();
        WdfRequestComplete(Request, Status);
        return;
    }

    // Release previous buffer if set
    if( queueContext->WriteMemory != NULL ) {
        WdfObjectDelete(queueContext->WriteMemory);
        queueContext->WriteMemory = NULL;
    }

    Status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES,
                             NonPagedPoolNx,
                             'sam1',
                             Length,
                             &queueContext->WriteMemory,
                             &writeBuffer
                             );

    if(!NT_SUCCESS(Status)) {
        KdPrint(("EchoEvtIoWrite: Could not allocate %d byte buffer\n", Length));
        WdfRequestComplete(Request, STATUS_INSUFFICIENT_RESOURCES);
        return;
    }


    // Copy the memory in
    Status = WdfMemoryCopyToBuffer( memory,
                                    0,  // offset into the source memory
                                    writeBuffer,
                                    Length );
    if( !NT_SUCCESS(Status) ) {
        KdPrint(("EchoEvtIoWrite WdfMemoryCopyToBuffer failed 0x%x\n", Status));
        WdfVerifierDbgBreakPoint();

        WdfObjectDelete(queueContext->WriteMemory);
        queueContext->WriteMemory = NULL;

        WdfRequestComplete(Request, Status);
        return;
    }

    // Set transfer information
    WdfRequestSetInformation(Request, (ULONG_PTR)Length);

    // Specify the request is cancelable
    WdfRequestMarkCancelable(Request, EchoEvtRequestCancel);

    // Defer the completion to another thread from the timer dpc
    queueContext->CurrentRequest = Request;
    queueContext->CurrentStatus  = Status;

    return;
}


VOID
EchoEvtTimerFunc(
    IN WDFTIMER     Timer
    )
/*++

Routine Description:

    This is the TimerDPC the driver sets up to complete requests.
    This function is registered when the WDFTIMER object is created, and
    will automatically synchronize with the I/O Queue callbacks
    and cancel routine.

Arguments:

    Timer - Handle to a framework Timer object.

Return Value:

    VOID

--*/
{
    NTSTATUS      Status;
    WDFREQUEST     Request;
    WDFQUEUE queue;
    PQUEUE_CONTEXT queueContext ;

    queue = WdfTimerGetParentObject(Timer);
    queueContext = QueueGetContext(queue);

    //
    // DPC is automatically synchronized to the Queue lock,
    // so this is race free without explicit driver managed locking.
    //
    Request = queueContext->CurrentRequest;
    if( Request != NULL ) {

        //
        // Attempt to remove cancel status from the request.
        //
        // The request is not completed if it is already cancelled
        // since the EchoEvtIoCancel function has run, or is about to run
        // and we are racing with it.
        //
        Status = WdfRequestUnmarkCancelable(Request);
        if( Status != STATUS_CANCELLED ) {

            queueContext->CurrentRequest = NULL;
            Status = queueContext->CurrentStatus;

            KdPrint(("CustomTimerDPC Completing request 0x%p, Status 0x%x \n", Request,Status));

            WdfRequestComplete(Request, Status);
        }
        else {
            KdPrint(("CustomTimerDPC Request 0x%p is STATUS_CANCELLED, not completing\n",
                                Request));
        }
    }

    //
    // Restart the Timer since WDF does not allow periodic timer
    // with autosynchronization at passive level
    //
    WdfTimerStart(Timer, WDF_REL_TIMEOUT_IN_MS(TIMER_PERIOD));

    return;
}


