#include "GenFilter.h"

NTSTATUS
DriverEntry(PDRIVER_OBJECT  DriverObject,
            PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS          status;

#if DBG
    DbgPrint("GenFilter...Compiled %s %s\n",
             __DATE__,
             __TIME__);
#endif

    WDF_DRIVER_CONFIG_INIT(&config,
                           GenFilterEvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &config,
                             WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) {
#if DBG
        DbgPrint("WdfDriverCreate failed - 0x%x\n",
                 status);
#endif
        goto done;
    }

    status = STATUS_SUCCESS;

done:

    return status;
}

NTSTATUS
GenFilterEvtDeviceAdd(WDFDRIVER       Driver,
                      PWDFDEVICE_INIT DeviceInit)
{
    NTSTATUS                  status;
    WDF_OBJECT_ATTRIBUTES     wdfObjectAttr;
    WDFDEVICE                 wdfDevice;
    PGENFILTER_DEVICE_CONTEXT devContext;
    WDF_IO_QUEUE_CONFIG       ioQueueConfig;

#if DBG
    DbgPrint("GenFilterEvtDeviceAdd: Adding device...\n");
#endif

    UNREFERENCED_PARAMETER(Driver);

    //
    // Indicate that we're creating a FILTER Device, as opposed to a FUNCTION Device.
    //
    // This will cause the Framework to attach us correctly to the device stack,
    // auto-forward any requests we don't explicitly handle, and use the
    // appropriate state machine for PnP/Power management among
    // several other things.
    //
    WdfFdoInitSetFilter(DeviceInit);

    //
    // Setup our device attributes specifying our per-Device context
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfObjectAttr,
                                            GENFILTER_DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit,
                             &wdfObjectAttr,
                             &wdfDevice);

    if (!NT_SUCCESS(status)) {
#if DBG
        DbgPrint("WdfDeviceCreate failed - 0x%x\n",
                 status);
#endif
        goto done;
    }

    //
    // Save our WDFDEVICE handle in the Device context for convenience
    //
    devContext = GenFilterGetDeviceContext(wdfDevice);
    devContext->WdfDevice = wdfDevice;

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
    ioQueueConfig.EvtIoRead          = GenFilterEvtRead;
    ioQueueConfig.EvtIoWrite         = GenFilterEvtWrite;
    ioQueueConfig.EvtIoDeviceControl = GenFilterEvtDeviceControl;

    //
    // Create the queue...
    //
    status = WdfIoQueueCreate(devContext->WdfDevice,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) {
#if DBG
        DbgPrint("WdfIoQueueCreate failed - 0x%x\n",
                 status);
#endif
        goto done;
    }

    status = STATUS_SUCCESS;

done:

    return status;
}

VOID
GenFilterEvtDeviceControl(WDFQUEUE   Queue,
                          WDFREQUEST Request,
                          size_t     OutputBufferLength,
                          size_t     InputBufferLength,
                          ULONG      IoControlCode)
{
    PGENFILTER_DEVICE_CONTEXT devContext;

    devContext = GenFilterGetDeviceContext(WdfIoQueueGetDevice(Queue));

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

#if DBG
    DbgPrint("GenFilterEvtDeviceControl -- Request 0x%p\n",
             Request);
#endif

    //
    // We're searching for one specific IOCTL function code that we're interested in
    //
    if (IoControlCode == IOCTL_YOU_ARE_INTERESTED_IN) {

#if DBG
        DbgPrint("GenFilterEvtDeviceControl -- The IOCTL we're looking for was found!  Request 0x%p\n",
                 Request);
#endif
        //
        // Do something useful.
        //

        //
        // We want to see the results for this particular Request... so send it
        // and request a callback for when the Request has been completed.
        GenFilterSendWithCallback(Request,
                                    devContext);

        return;
    }

    GenFilterSendAndForget(Request,
                           devContext);
}

VOID
GenFilterEvtRead(WDFQUEUE   Queue,
                 WDFREQUEST Request,
                 size_t     Length)
{
    PGENFILTER_DEVICE_CONTEXT devContext;

    UNREFERENCED_PARAMETER(Length);

    devContext = GenFilterGetDeviceContext(WdfIoQueueGetDevice(Queue));

#if DBG
    DbgPrint("GenFilterEvtRead -- Request 0x%p\n",
             Request);
#endif

    GenFilterSendAndForget(Request,
                           devContext);
}

_Use_decl_annotations_
VOID
GenFilterEvtWrite(WDFQUEUE   Queue,
                  WDFREQUEST Request,
                  size_t     Length)
{
    PGENFILTER_DEVICE_CONTEXT devContext;

    UNREFERENCED_PARAMETER(Length);

    devContext = GenFilterGetDeviceContext(WdfIoQueueGetDevice(Queue));

#if DBG
    DbgPrint("GenFilterEvtWrite -- Request 0x%p\n",
             Request);
#endif

    GenFilterSendAndForget(Request,
                           devContext);
}

VOID
GenFilterSendAndForget(WDFREQUEST                Request,
                       PGENFILTER_DEVICE_CONTEXT DevContext)
{
    NTSTATUS status;

    WDF_REQUEST_SEND_OPTIONS sendOpts;

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
GenFilterCompletionCallback(WDFREQUEST                     Request,
                            WDFIOTARGET                    Target,
                            PWDF_REQUEST_COMPLETION_PARAMS Params,
                            WDFCONTEXT                     Context)
{
    NTSTATUS status;
    auto*    devContext = (PGENFILTER_DEVICE_CONTEXT)Context;

    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(devContext);

    DbgPrint("GenFilterCompletionCallback: Request=%p, Status=0x%x; Information=0x%Ix\n",
             Request,
             Params->IoStatus.Status,
             Params->IoStatus.Information);

    status = Params->IoStatus.Status;

    //
    // Potentially do something interesting here
    //

    WdfRequestComplete(Request,
                       status);
}

_Use_decl_annotations_
VOID
GenFilterSendWithCallback(WDFREQUEST                Request,
                          PGENFILTER_DEVICE_CONTEXT DevContext)
{
    NTSTATUS status;

    DbgPrint("Sending %p with completion\n",
             Request);

    //
    // Setup the request for the next driver
    //
    WdfRequestFormatRequestUsingCurrentType(Request);

    //
    // Set the completion routine...
    //
    WdfRequestSetCompletionRoutine(Request,
                                   GenFilterCompletionCallback,
                                   DevContext);
    //
    // And send it!
    // 
    if (!WdfRequestSend(Request,
                        WdfDeviceGetIoTarget(DevContext->WdfDevice),
                        WDF_NO_SEND_OPTIONS)) {

        //
        // Oops! Something bad happened, complete the request
        //
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
