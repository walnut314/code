#include <ntddk.h>
#include <wdf.h>
#include <wdmsec.h> // for SDDLs
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#define DRIVERNAME "Generic.sys: "

//
// Change the following define to 1 if you want to forward
// the request with a completion routine.
//
#define FORWARD_REQUEST_WITH_COMPLETION 0


typedef struct _FILTER_EXTENSION
{
    WDFDEVICE WdfDevice;
} FILTER_EXTENSION, *PFILTER_EXTENSION;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FILTER_EXTENSION, FilterGetData)

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD FilterEvtDeviceAdd;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL FilterEvtIoDeviceControl;

VOID FilterForwardRequest( IN WDFREQUEST Request, IN WDFIOTARGET Target);

#if FORWARD_REQUEST_WITH_COMPLETION

VOID FilterForwardRequestWithCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target);
VOID FilterRequestCompletionRoutine(IN WDFREQUEST Request, IN WDFIOTARGET Target,
    PWDF_REQUEST_COMPLETION_PARAMS CompletionParams, IN WDFCONTEXT Context);

#endif //FORWARD_REQUEST_WITH_COMPLETION

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, FilterEvtDeviceAdd)
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    WDF_DRIVER_CONFIG   config;
    NTSTATUS            status;
    WDFDRIVER           hDriver;

    KdPrint(("Toaster Generic Filter Driver Sample - Driver Framework Edition.\n"));
    WDF_DRIVER_CONFIG_INIT( &config, FilterEvtDeviceAdd);
    // Create a framework driver object to represent our driver.
    status = WdfDriverCreate(DriverObject,
                            RegistryPath,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            &config,
                            &hDriver);
    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfDriverCreate failed with status 0x%x\n", status));
    }
    return status;
}

NTSTATUS
FilterEvtDeviceAdd(
    IN WDFDRIVER        Driver,
    IN PWDFDEVICE_INIT  DeviceInit
    )
/*++
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
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    PFILTER_EXTENSION       filterExt;
    NTSTATUS                status;
    WDFDEVICE               device;    
    WDF_IO_QUEUE_CONFIG     ioQueueConfig;

    PAGED_CODE ();

    UNREFERENCED_PARAMETER(Driver);
    // Tell the framework that you are filter driver. Framework
    // takes care of inherting all the device flags & characterstics
    // from the lower device you are attaching to.
    WdfFdoInitSetFilter(DeviceInit);
    // Specify the size of device extension where we track per device
    // context.
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, FILTER_EXTENSION);
    // Create a framework device object.This call will inturn create
    // a WDM deviceobject, attach to the lower stack and set the
    // appropriate flags and attributes.
    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfDeviceCreate failed with status code 0x%x\n", status));
        return status;
    }
    filterExt = FilterGetData(device);

    // Configure the default queue to be Parallel. 
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                             WdfIoQueueDispatchParallel);

    // Framework by default creates non-power managed queues for
    // filter drivers.
    ioQueueConfig.EvtIoDeviceControl = FilterEvtIoDeviceControl;

    status = WdfIoQueueCreate(device,
                            &ioQueueConfig,
                            WDF_NO_OBJECT_ATTRIBUTES,
                            WDF_NO_HANDLE // pointer to default queue
                            );
    if (!NT_SUCCESS(status)) {
        KdPrint( ("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }   
    return status;
}

VOID
FilterEvtIoDeviceControl(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
    )
/*++
Routine Description:
    This routine is the dispatch routine for internal device control requests.
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
    PFILTER_EXTENSION               filterExt;
    NTSTATUS                        status = STATUS_SUCCESS;
    WDFDEVICE                       device;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    filterExt = FilterGetData(device);
    switch (IoControlCode) {
    //
    // Put your cases for handling IOCTLs here
    //
    }

    if (!NT_SUCCESS(status)) {
        WdfRequestComplete(Request, status);
        return;
    }

    // Forward the request down. WdfDeviceGetIoTarget returns
    // the default target, which represents the device attached to us below in
    // the stack.
#if FORWARD_REQUEST_WITH_COMPLETION
    // Use this routine to forward a request if you are interested in post
    // processing the IRP.
        FilterForwardRequestWithCompletionRoutine(Request, 
                                               WdfDeviceGetIoTarget(device));
#else   
        FilterForwardRequest(Request, WdfDeviceGetIoTarget(device));
#endif

    return;
}

VOID
FilterForwardRequest(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target
    )
/*++
Routine Description:

    Passes a request on to the lower driver.

--*/
{
    WDF_REQUEST_SEND_OPTIONS options;
    BOOLEAN ret;
    NTSTATUS status;

    //
    // We are not interested in post processing the IRP so 
    // fire and forget.
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(&options, WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    ret = WdfRequestSend(Request, Target, &options);

    if (ret == FALSE) {
        status = WdfRequestGetStatus (Request);
        KdPrint( ("WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }

    return;
}

#if FORWARD_REQUEST_WITH_COMPLETION

VOID
FilterForwardRequestWithCompletionRoutine(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target
    )
/*++
Routine Description:
    This routine forwards the request to a lower driver with
    a completion so that when the request is completed by the
    lower driver, it can regain control of the request and look
    at the result.
--*/
{
    BOOLEAN ret;
    NTSTATUS status;

    // The following funciton essentially copies the content of
    // current stack location of the underlying IRP to the next one. 
    WdfRequestFormatRequestUsingCurrentType(Request);

    WdfRequestSetCompletionRoutine(Request,
                                FilterRequestCompletionRoutine,
                                WDF_NO_CONTEXT);

    ret = WdfRequestSend(Request,
                         Target,
                         WDF_NO_SEND_OPTIONS);

    if (ret == FALSE) {
        status = WdfRequestGetStatus (Request);
        KdPrint( ("WdfRequestSend failed: 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }

    return;
}

VOID
FilterRequestCompletionRoutine(
    IN WDFREQUEST                  Request,
    IN WDFIOTARGET                 Target,
    PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
    IN WDFCONTEXT                  Context
   )
/*++
Routine Description:
    Completion Routine
Arguments:
    Target - Target handle
    Request - Request handle
    Params - request completion params
    Context - Driver supplied context
Return Value:
    VOID
--*/
{
    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(Context);

    WdfRequestComplete(Request, CompletionParams->IoStatus.Status);

    return;
}

#endif //FORWARD_REQUEST_WITH_COMPLETION

