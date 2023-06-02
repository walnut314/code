#include "pstfilt.h"

//
// The trace message header file must be included in a source file
// before any WPP macro calls and after defining a WPP_CONTROL_GUIDS
// macro. During the compilation, WPP scans the source files for
// TraceEvents() calls and builds a .tmh file which stores a unique
// data GUID for each message, the text resource string for each message,
// and the data types of the variables passed in for each message.
// This file is automatically generated and used during post-processing.
//
#include "pstfilt.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT, DriverEntry )
#pragma alloc_text( PAGE, PstDriverUnload)
#pragma alloc_text( PAGE, PstEvtDriverUnload)
#pragma alloc_text( PAGE, PstDeviceAdd)
#pragma alloc_text( PAGE, PstEvtDeviceContextCleanup)
#pragma alloc_text( PAGE, PstControlDeviceAdd)
#pragma alloc_text( PAGE, PstEvtDeviceControl)
#pragma alloc_text( PAGE, PstEvtDeviceWdmIrpPreprocess)
#pragma alloc_text( PAGE, PstSendAndForget)
#pragma alloc_text( PAGE, PstSendWithCallback)
#pragma alloc_text( PAGE, PstCompletionCallback)
#pragma alloc_text( PAGE, FileEvtIoDeviceControl)
#endif // ALLOC_PRAGMA

// GLOBALS
GLOBAL_PST  globals;
//

NTSTATUS
DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
    )
{
    NTSTATUS              status;
    WDF_DRIVER_CONFIG     config;
    WDFDRIVER             hDriver;
    PWDFDEVICE_INIT       pInit = NULL;
    WDF_OBJECT_ATTRIBUTES attributes;

#if DBG
    DbgPrint("Pst...Compiled %s %s\n",
             __DATE__,
             __TIME__);
#endif

    globals.ndevs = 0;
    globals.fail = WdfFalse;
    KeInitializeMutex(&globals.lock, 0);

    WDF_DRIVER_CONFIG_INIT(&config, PstDeviceAdd);

    config.EvtDriverUnload = PstEvtDriverUnload;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             &hDriver);

    if (!NT_SUCCESS(status)) {

        KdPrint(("WdfDriverCreate failed - 0x%x\n", status));
        goto done;
    }

    //
    //
    // In order to create a control device, we first need to allocate a
    // WDFDEVICE_INIT structure and set all properties.
    //
    pInit = WdfControlDeviceInitAllocate(
                            hDriver,
                            &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R
                            );

    if (pInit == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto done;
    }

    DriverObject->DriverUnload = PstDriverUnload;

    status = PstControlDeviceAdd(hDriver, pInit);
done:

    return status;
}

VOID
PstEvtDriverUnload(
    IN WDFDRIVER Driver
    )
{
    UNREFERENCED_PARAMETER(Driver);
    PAGED_CODE();
}

VOID
PstDriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);
    PAGED_CODE();
}

NTSTATUS
PstControlDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    )
{
    NTSTATUS                  status;
    WDF_OBJECT_ATTRIBUTES     attributes;
    WDF_IO_QUEUE_CONFIG       ioQueueConfig;
    WDFQUEUE                  queue;
    WDFDEVICE                 controlDevice;
    
    DECLARE_CONST_UNICODE_STRING(ntDeviceName, NTDEVICE_NAME_STRING) ;
    DECLARE_CONST_UNICODE_STRING(symbolicLinkName, SYMBOLIC_NAME_STRING) ;
    
    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    WdfDeviceInitSetExclusive(DeviceInit, TRUE);
    WdfDeviceInitSetIoType(DeviceInit, WdfDeviceIoBuffered);

    status = WdfDeviceInitAssignName(DeviceInit, &ntDeviceName);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceInitAssignName failed 0x%x", status));
        goto End;
    }

    // Specify the size of device context
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, CONTROL_DEVICE_EXTENSION);

    status = WdfDeviceCreate(&DeviceInit,
                             &attributes,
                             &controlDevice);

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceCreate failed 0x%x", status));
        goto End;
    }

    //
    // Create a symbolic link for the control object so that usermode can open
    // the device.
    //
    status = WdfDeviceCreateSymbolicLink(controlDevice,
                                &symbolicLinkName);

    if (!NT_SUCCESS(status)) {
        //
        // Control device will be deleted automatically by the framework.
        //
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfDeviceCreateSymbolicLink failed %!STATUS!", status);
        goto End;
    }

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                    WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoDeviceControl = FileEvtIoDeviceControl;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    __analysis_assume(ioQueueConfig.EvtIoStop != 0);
    status = WdfIoQueueCreate(controlDevice,
                              &ioQueueConfig,
                              &attributes,
                              &queue // pointer to default queue
                              );
    __analysis_assume(ioQueueConfig.EvtIoStop == 0);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfIoQueueCreate failed 0x%x", status));
        goto End;
    }
    
    // Control devices must notify WDF when they are done initializing.   I/O is
    // rejected until this call is made.
    //
    WdfControlFinishInitializing(controlDevice);

End:    
    //
    // If the device is created successfully, framework would clear the
    // DeviceInit value. Otherwise device create must have failed so we
    // should free the memory ourself.
    //
    if (DeviceInit != NULL) {
        WdfDeviceInitFree(DeviceInit);
    }
    return status;
}

NTSTATUS
PstDeviceAdd(
    IN WDFDRIVER Driver,
    IN PWDFDEVICE_INIT DeviceInit
    )
{
    NTSTATUS                        status;
    WDF_OBJECT_ATTRIBUTES           attributes;
    WDFDEVICE                       wdfDevice;
    PPST_DEVICE_CONTEXT             devContext = NULL;
    WDF_IO_QUEUE_CONFIG             ioQueueConfig;

    KdPrint(("PstEvtDeviceAdd: Adding device...\n"));

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Driver);

    UCHAR MinorFunctionTable[NUM_PNP_CALLBACKS] = {IRP_MN_QUERY_DEVICE_RELATIONS,
                                                   IRP_MN_REMOVE_DEVICE,
                                                   IRP_MN_SURPRISE_REMOVAL};

    // create a WDM Irp callback for Pnp
    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(DeviceInit, 
                                                         PstEvtDeviceWdmIrpPreprocess,
                                                         IRP_MJ_PNP,
                                                         MinorFunctionTable,
                                                         NUM_PNP_CALLBACKS);
    
    // Indicate that we're creating a FILTER Device, as opposed to a FUNCTION Device.
    //
    // This will cause the Framework to attach us correctly to the device stack,
    // auto-forward any requests we don't explicitly handle, and use the
    // appropriate state machine for PnP/Power management among
    // several other things.
    //
    WdfFdoInitSetFilter(DeviceInit);

    // Setup our device attributes specifying our per-Device context
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes,
                                            PST_DEVICE_CONTEXT);

    attributes.EvtCleanupCallback = PstEvtDeviceContextCleanup;

    status = WdfDeviceCreate(&DeviceInit,
                             &attributes,
                             &wdfDevice);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfDeviceCreate failed %!STATUS!", status);
        goto Cleanup;
    }

    // Save our WDFDEVICE handle in the Device context for convenience
    //
    devContext = PstGetDeviceContext(wdfDevice);
    devContext->WdfDevice = wdfDevice;
    devContext->DeviceInit = DeviceInit;

    //
    // Create our default Queue -- This is how we receive Requests.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                           WdfIoQueueDispatchParallel);

    //
    // Specify callbacks for those Requests that we want this driver to "see."
    //
    // Note that because this driver is a FILTER, if we do not specify a
    // callback for a particular Request type the Framework will automatically
    // forward all Requests that it receives of that type to our Local I/O
    // Target.  So, for example, if you're not interested in inspecting
    // READ Requests, you can just not specify an EvtIoRead callback and the
    // Framework will "do the right thing" and send it along.
    //
    ioQueueConfig.EvtIoDeviceControl = PstEvtDeviceControl;

    //
    // Create the queue...
    //
    status = WdfIoQueueCreate(devContext->WdfDevice,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfIoQueueCreate failed %!STATUS!", status);
        goto Cleanup;
    }

Cleanup:

    if (!NT_SUCCESS(status)) {
        
        if (DeviceInit != NULL) {
            WdfDeviceInitFree(DeviceInit);
        }

        if (wdfDevice) {
            WdfObjectDelete(wdfDevice);
        }
    } else {
        KeWaitForSingleObject(&globals.lock, Executive, KernelMode, FALSE, NULL);
        globals.devs[globals.ndevs] = devContext;
        globals.ndevs++;
        KeReleaseMutex(&globals.lock, FALSE);
    }

    return status;
}

VOID PstEvtDeviceContextCleanup(
    IN WDFOBJECT Device
)
{
    PPST_DEVICE_CONTEXT     devContext;

    PAGED_CODE();
    devContext = PstGetDeviceContext(Device);

    if (devContext->DeviceInit != NULL) {
        WdfDeviceInitFree(devContext->DeviceInit);
    }

    if (devContext->WdfDevice) {
        WdfObjectDelete(devContext->WdfDevice);
    }
    
}

NTSTATUS PstEvtDeviceWdmIrpPreprocess(
    IN WDFDEVICE Device,
    IN PIRP Irp
)
{
    NTSTATUS status;
    PIO_STACK_LOCATION irpStack;
    DEVICE_RELATION_TYPE type;
    UCHAR minorFunction;
    PDEVICE_RELATIONS relations;

    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(Irp);

    PAGED_CODE();

    irpStack = IoGetCurrentIrpStackLocation(Irp);    
    minorFunction = irpStack->MinorFunction;

    switch (minorFunction) {
        case IRP_MN_QUERY_DEVICE_RELATIONS:
            KdPrint(("IRP_MN_QUERY_DEVICE_RELATIONS"));
            type = irpStack->Parameters.QueryDeviceRelations.Type;
            if (type == BusRelations) {
                // forward the request with callback - or send synchronously
                relations = (PDEVICE_RELATIONS) Irp->IoStatus.Information;
                if (relations) {
                    for (ULONG i = 0; i < relations->Count; i++) {
                        PDEVICE_OBJECT obj = relations->Objects[i];
                        KdPrint(("obj -> %p", obj));
                    }
                }
            }
            break;
        case IRP_MN_REMOVE_DEVICE:
            KdPrint(("IRP_MN_REMOVE_DEVICE"));
            KeWaitForSingleObject(&globals.lock, Executive, KernelMode, FALSE, NULL);
            if (globals.ndevs) {
                globals.ndevs--;
                globals.devs[globals.ndevs] = NULL;
            }
            KeReleaseMutex(&globals.lock, FALSE);
            break;
        case IRP_MN_SURPRISE_REMOVAL:
            KdPrint(("IRP_MN_SURPRISE_REMOVAL"));
            KeWaitForSingleObject(&globals.lock, Executive, KernelMode, FALSE, NULL);
            if (globals.ndevs) {
                globals.ndevs--;
                globals.devs[globals.ndevs] = NULL;
            }
            KeReleaseMutex(&globals.lock, FALSE);
            break;
        default:
            break;
    }

    status = STATUS_SUCCESS;

    return status;
}

VOID
FileEvtIoDeviceControl(
    IN WDFQUEUE         Queue,
    IN WDFREQUEST       Request,
    IN size_t           OutputBufferLength,
    IN size_t           InputBufferLength,
    IN ULONG            IoControlCode
    )
{
    NTSTATUS            status;
    PPST_DEVICE_CONTEXT devContext;
    PDEVICE_OBJECT      pdo;

    UNREFERENCED_PARAMETER(Queue);

    PAGED_CODE();

    if(!OutputBufferLength || !InputBufferLength)
    {
        status = STATUS_INVALID_PARAMETER;
        goto End;
    }

    switch (IoControlCode)
    {
    case IOCTL_PST_METHOD_FAIL_STACK:
        KdPrint(("ioctl fail recv'd\n"));
        KeWaitForSingleObject(&globals.lock, Executive, KernelMode, FALSE, NULL);
        globals.fail = WdfTrue;
        devContext = globals.devs[globals.ndevs-1];
        KeReleaseMutex(&globals.lock, FALSE);        
        pdo = WdfDeviceWdmGetPhysicalDevice(devContext->WdfDevice);
        IoInvalidateDeviceRelations(pdo, BusRelations);
        status = STATUS_SUCCESS;
        break;
    case IOCTL_PST_METHOD_RESTORE_STACK:
        KdPrint(("ioctl restore recv'd\n"));
        KeWaitForSingleObject(&globals.lock, Executive, KernelMode, FALSE, NULL);
        globals.fail = WdfFalse;
        devContext = globals.devs[globals.ndevs-1];
        KeReleaseMutex(&globals.lock, FALSE);        
        pdo = WdfDeviceWdmGetPhysicalDevice(devContext->WdfDevice);
        IoInvalidateDeviceRelations(pdo, BusRelations);
        status = STATUS_SUCCESS;
        break;
    default:
        //
        // The specified I/O control code is unrecognized by this driver.
        //
        status = STATUS_INVALID_DEVICE_REQUEST;
        KdPrint(("ERROR: unrecognized IOCTL %x\n", IoControlCode));
        break;
        
    }

End:
    WdfRequestComplete(Request, status);

}

VOID
PstEvtDeviceControl(WDFQUEUE   Queue,
                          WDFREQUEST Request,
                          size_t     OutputBufferLength,
                          size_t     InputBufferLength,
                          ULONG      IoControlCode)
{
    PPST_DEVICE_CONTEXT devContext;

    PAGED_CODE();

    devContext = PstGetDeviceContext(WdfIoQueueGetDevice(Queue));

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

#if DBG
    DbgPrint("GenFilterEvtDeviceControl -- Request 0x%p\n",
             Request);
#endif

    //
    // We're searching for one specific IOCTL function code that we're interested in
    //
    if (IoControlCode == IOCTL_PST_METHOD_FAIL_STACK) {

#if DBG
        DbgPrint("PstEvtDeviceControl -- The IOCTL we're looking for was found!  Request 0x%p\n",
                 Request);
#endif
        // Do something useful.
        //
        //
        // We want to see the results for this particular Request... so send it
        // and request a callback for when the Request has been completed.
        PstSendWithCallback(Request, devContext);

        return;
    }

    PstSendAndForget(Request,
                           devContext);
}

VOID
PstSendAndForget(IN WDFREQUEST                Request,
                 IN PPST_DEVICE_CONTEXT       DevContext)
{
    NTSTATUS status;

    WDF_REQUEST_SEND_OPTIONS sendOpts;

    PAGED_CODE();
    
    //
    // We want to send this Request and not deal with it again.  Note two
    // important things about send-and-forget:
    //
    // 1. Sending a Request with send-and-forget is the logical equivalent of
    //    completing the Request for the sending driver.  If WdfRequestSend returns
    //    TRUE, the Request is no longer owned by the sending driver.
    //
    // 2.  Send-and-forget is pretty much restricted to use only with the Local I/O Target.
    //     That's how we use it here.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOpts,
                                  WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    if (!WdfRequestSend(Request,
                        WdfDeviceGetIoTarget(DevContext->WdfDevice),
                        &sendOpts)) {

        //
        // Oops! The Framework was unable to give the Request to the specified
        // I/O Target.  Note that getting back TRUE from WdfRequestSend does not
        // imply that the I/O Target processed the Request with an ultimate status
        // of STATUS_SUCCESS. Rather, WdfRequestSend returning TRUE simply means
        // that the Framework was successful in delivering the Request to the
        // I/O Target for processing by the driver for that Target.
        //
        status = WdfRequestGetStatus(Request);
#if DBG
        DbgPrint("WdfRequestSend 0x%p failed - 0x%x\n",
                 Request,
                 status);
#endif
        WdfRequestComplete(Request,
                           status);
    }
}

VOID
PstCompletionCallback(IN WDFREQUEST                     Request,
                      IN WDFIOTARGET                    Target,
                      IN PWDF_REQUEST_COMPLETION_PARAMS Params,
                      IN WDFCONTEXT                     Context)
{
    NTSTATUS status;

    PAGED_CODE();

    PPST_DEVICE_CONTEXT devContext = (PPST_DEVICE_CONTEXT)Context;


    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(devContext);

    DbgPrint("PstCompletionCallback: Request=%p, Status=0x%x; Information=0x%Ix\n",
             Request,
             Params->IoStatus.Status,
             Params->IoStatus.Information);

    status = Params->IoStatus.Status;

    // Potentially do something interesting here
    //

    WdfRequestComplete(Request,
                       status);
}

VOID
PstSendWithCallback(IN WDFREQUEST                Request,
                    IN PPST_DEVICE_CONTEXT       DevContext)
{
    NTSTATUS status;

    PAGED_CODE();
    
    DbgPrint("Sending %p with completion\n",
             Request);

    // Setup the request for the next driver
    WdfRequestFormatRequestUsingCurrentType(Request);

    // Set the completion routine...
    WdfRequestSetCompletionRoutine(Request,
                                   PstCompletionCallback,
                                   DevContext);
    // And send it!
    if (!WdfRequestSend(Request,
                        WdfDeviceGetIoTarget(DevContext->WdfDevice),
                        WDF_NO_SEND_OPTIONS)) {

        // Oops! Something bad happened, complete the request
        status = WdfRequestGetStatus(Request);

        DbgPrint("WdfRequestSend failed = 0x%x\n",
                 status);
        WdfRequestComplete(Request,
                           status);
    }

    //
    // When we return the Request is always "gone"
    //
}
