///////////////////////////////////////////////////////////////////////////////
//
//    (C) Copyright 1995 - 2006 OSR Open Systems Resources, Inc.
//    All Rights Reserved
//
//    This sofware is supplied for instructional purposes only.
//
//    OSR Open Systems Resources, Inc. (OSR) expressly disclaims any warranty
//    for this software.  THIS SOFTWARE IS PROVIDED  "AS IS" WITHOUT WARRANTY
//    OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION,
//    THE IMPLIED WARRANTIES OF MECHANTABILITY OR FITNESS FOR A PARTICULAR
//    PURPOSE.  THE ENTIRE RISK ARISING FROM THE USE OF THIS SOFTWARE REMAINS
//    WITH YOU.  OSR's entire liability and your exclusive remedy shall not
//    exceed the price paid for this material.  In no event shall OSR or its
//    suppliers be liable for any damages whatsoever (including, without
//    limitation, damages for loss of business profit, business interruption,
//    loss of business information, or any other pecuniary loss) arising out
//    of the use or inability to use this software, even if OSR has been
//    advised of the possibility of such damages.  Because some states/
//    jurisdictions do not allow the exclusion or limitation of liability for
//    consequential or incidental damages, the above limitation may not apply
//    to you.
//
//    OSR Open Systems Resources, Inc.
//    105 Route 101A Suite 19
//    Amherst, NH 03031  (603) 595-6500 FAX: (603) 595-6503
//    email bugs to: bugs@osr.com
//
//
//    MODULE:
//
//      wdffltr.cpp
//
//    ABSTRACT:
//
//      This file contains the all of the code necessary for a WDF filter
//      driver, including DriverEntry, EvtDeviceAdd, and various WDF 
//      callbacks for filtering purposes
//
//    AUTHOR(S):
//
//      OSR Open Systems Resources, Inc.
// 
//    REVISION:   
//
//
///////////////////////////////////////////////////////////////////////////////
#include "wdffltr.h"

/////////////////////////////////////////////////////////////////////
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// +          PLEASE READ ME! LIKE, SERIOUSLY, NO JOKE!          + //
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
//                                                                 //
// 02/23/06                                                        //
//                                                                 //
// After the article that this code accompanies was published, a   //
//  bug was found in WDF surrounding control devices and filters.  //
//                                                                 //
// The filter driver examples in the WDF distribution indicate     //
//  that the proper way to clean up a control device is to         //
//  call WdfObjectDelete on it when the last filter device goes    //
//  away. Unfortunately, this is entirely unsafe and prone to      //
//  BSODing. The problem is that the call to WdfObjectDelete       //
//  completely tears down the control device, regardless of        //
//  whether or not a user mode application has an open HANDLE to   //
//  it. If the user mode application then proceeds to do ANY       //
//  operations against the open HANDLE, the system will BSOD when  //
//  the framework accesses the (now dead) WDFDEVICE.               //
//                                                                 //
// Currently there is no 100% solution to this aside from not      //
//  allowing the filter driver to unload if it creates a control   //
//  device. One can make the window under which the system would   //
//  crash sufficiently small so that a crash is unlikely, but this //
//  sample chooses to leave that risk up to the informed reader.   //
//                                                                 //
// Therefore, this sample will delete the control device and       //
//  unload in the checked build if no one currently has an open    //
//  HANDLE to the control device. If the control device is         //
//  open when the last filtered device is going away, then the     //
//  removal of the last device is rejected. In the free build      //
//  this logic is removed and the filter is not unloadable. The    //
//  reason for this is that even if one synchronizes the           //
//  EvtCreate callback with the removal callback to properly       //
//  serialize against an opening of the control device and the     //
//  removal of the last device, there is no guarantee that the     //
//  create won't arrive immediately AFTER the delete of the        //
//  control device, since the framework might have been in the     //
//  act of preprocessing the create when the last remove           //
//  occurred. And, remember, the WDFDEVICE is GONE after the       //
//  WdfObjectDelete, therefore the framework will end up working   //
//  with freed memory                                              //
//                                                                 //
// (And, no, you can't delete the control device from within the   //
//  EvtClose handler (or a work item queued from the EvtClose      //
//  handler) so don't even bother trying to cook up a solution     //
//  that way :))                                                   //
//                                                                 //
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
// +                  YOU READ THAT, RIGHT??                     + //
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //
/////////////////////////////////////////////////////////////////////

//////////////////////
// GLOBAL VARIABLES //
//////////////////////

WDFFLTR_REQUEST_BUFFER_TRACKER WdfFltrRequestBufferTracker;
LONG                           WdfFltrDeviceCount = 0;
WDFDEVICE                      WdfFltrControlDevice = NULL;
BOOLEAN                        WdfFltrControlDeviceOpen;

///////////////////////////////////////////////////////////////////////////////
//
//  DriverEntry
//
//      This routine is called when the driver is first loaded. Our job will be 
//      to create our WDF driver object and also do any driver wide
//      initialization
//
//  INPUTS:
//
//      DriverObject - Address of the WDM driver object
//
//      RegistryPath - Our driver's service key
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      STATUS_SUCCESS. Otherwise an error indicating why the driver could not
//      load.
//
//  IRQL:
//
//    PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
extern "C"
NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
    ) {

    WDF_DRIVER_CONFIG config;
    NTSTATUS          status;


    WdfFltrTrace(("WDF Filter Driver...Compiled %s %s\n" ,__DATE__, __TIME__));

    //
    // Initialize our driver config structure, specifying our 
    //  add device callback.
    //
    WDF_DRIVER_CONFIG_INIT(&config,
                           WdfFltrEvtDeviceAdd);

    //
    // We'll need to setup an unload entry point
    //  so we can free our global memory
    //
    config.EvtDriverUnload = DriverUnload;

    //
    // Create the framework driver object...
    //
    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &config,
                             NULL);
    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfDriverCreate failed - 0x%x\n", status));
        return status;
    }

    //
    // Initialize our global data
    //
    status = WdfFltrInitRequestBufferTracker(&WdfFltrRequestBufferTracker);

    if (!NT_SUCCESS(status)) {

        WdfFltrTrace(("WdfFltrInitRequestBufferTracker failed - 0x%x\n", 
                      status));
        return status;

    }


    return STATUS_SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////
//
//  DriverUnload
//
//      This routine is our dynamic unload entry point. 
//
//  INPUTS:
//
//      Driver - Address of our WDF driver object
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None.
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
DriverUnload(
    IN WDFDRIVER Driver
    ) {


    WdfFltrTrace(("Unloading...\n"));

    //
    // Cleanup any global data that exists
    //
    if (WdfFltrRequestBufferTracker.ActiveBuffer) {

        WdfFltrStopRequestBufferTracker(&WdfFltrRequestBufferTracker);
        ExFreePool(WdfFltrRequestBufferTracker.ActiveBuffer);

    }
    return;
}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrEvtDeviceAdd
//
//      This routine is called once for each instance of the type of device
//      that we filter in the system. Our job will be to create a WDF filter
//      device object and do any per device initialization. If this is the
//      first time that we're called at this routine, we'll also be creating
//      a control device that our user mode application will be talking to
//
//  INPUTS:
//
//      Driver     - Address of our WDF driver object
//
//      DeviceInit - The WDFDEVICE_INIT structure that, once we're done
//                   filling in, will describe the properties of our filter 
//                   device
//
//  OUTPUTS:
//
//      DeviceInit - We're responsible for filling in a lot of this structure,
//                   so it's also technically an output..
//
//  RETURNS:
//
//      An appropriate NTSTATUS code
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrEvtDeviceAdd(
    IN WDFDRIVER Driver,
    IN OUT PWDFDEVICE_INIT DeviceInit
    ) {

    NTSTATUS                     status;
    WDF_OBJECT_ATTRIBUTES        wdfObjectAttr;
    WDFDEVICE                    wdfDevice;
    PDEVICE_CONTEXT              devCont;
    WDF_IO_QUEUE_CONFIG          ioQueueConfig;
    WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;

    WdfFltrTrace(("Adding device...\n"));

    //
    // We're a filter, so mark our device init structure
    //  as such. This will do all sorts of lovely things,
    //  such as pass requests that we don't care about
    //  to the FDO unharmed...
    //
    WdfFdoInitSetFilter(DeviceInit);

    //
    // We don't know what our device type is going to be yet
    //  (we're an agnostic filter, remember?) So mark it as
    //  UNKNOWN. When we call WdfDeviceCreate the framework
    //  will do all the legwork for us to determine what the
    //  appropriate type is.
    //
    WdfDeviceInitSetDeviceType(DeviceInit, 
                               FILE_DEVICE_UNKNOWN);

    //
    // We want to log PnP operations in WDM terms,
    //  so we'll register for an IRP preprocessing
    //  callback for IRP_MJ_PNP
    //
    status = WdfDeviceInitAssignWdmIrpPreprocessCallback(
                                DeviceInit,
                                WdfFltrWdmPnp,
                                IRP_MJ_PNP,
                                NULL,
                                0);

    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfDeviceInitAssignWdmIrpPreprocessCallback "\
                      "failed - 0x%x\n", status));
        return status;
    }


    //
    // Let's log D0 entry and D0 Exit operations
    //
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

    pnpPowerCallbacks.EvtDeviceD0Entry = WdfFltrD0Entry;
    pnpPowerCallbacks.EvtDeviceD0Exit  = WdfFltrD0Exit;
    //
    // Setup a query remove callback so that the last device out
    //  can delete the control device that we'll be creating.
    //  This callback will also allow us to veto the remove of
    //  the last device if our control device is still open
    //
    pnpPowerCallbacks.EvtDeviceQueryRemove = WdfFltrQueryRemove;

    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit,
                                           &pnpPowerCallbacks);
                

    //
    // Setup our device attributes to have our context type
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfObjectAttr, 
                                            DEVICE_CONTEXT);


    //
    // And create our WDF device. This does a multitude of things,
    //  including:
    //
    //  1) Creating a WDM device object
    //  2) Attaching the device object to the filtered device object
    //  3) Propogates all of the flags and characteristics of the 
    //     target device to our filter device. So, for example, if 
    //     the target device is setup for direct I/O our filter 
    //     device will also be setup for direct I/O
    //
    status = WdfDeviceCreate(&DeviceInit, 
                             &wdfObjectAttr, 
                             &wdfDevice);

    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfDeviceCreate failed - 0x%x\n", status));
        return status;
    }

    //
    // Get our filter context
    //
    devCont = WdfFltrGetDeviceContext(wdfDevice);

    //
    // Initialize our context...
    //
    devCont->MagicNumber = DEVICE_CONTEXT_MAGIC;

    //
    // Figure out where we'll be sending all of our requests
    //  once we're done with them
    //
    devCont->TargetToSendRequestsTo = WdfDeviceGetIoTarget(wdfDevice);

    //
    // Now that that's over with, we can create our default queue.
    //  This queue will allow us to pick off any I/O requests
    //  that we may be interested in before they are forwarded
    //  to the target device.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                           WdfIoQueueDispatchParallel);

    //
    // Let's see...Definitely need reads...
    //
    ioQueueConfig.EvtIoRead = WdfFltrRead;

    //
    // Gotta have writes...
    //
    ioQueueConfig.EvtIoWrite = WdfFltrWrite;

    //
    // And why not some device controls...
    //
    ioQueueConfig.EvtIoDeviceControl 
                                = WdfFltrDeviceControl;
    ioQueueConfig.EvtIoInternalDeviceControl 
                                = WdfFltrInternalDeviceControl;

    //
    // Create the queue...
    //
    status = WdfIoQueueCreate(wdfDevice,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              NULL);

    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfIoQueueCreate failed - 0x%x\n", status));
        return status;
    }

    //
    // If this is the first device, then also
    //  create a control device object for our user
    //  mode app to use
    //
    if (InterlockedIncrement(&WdfFltrDeviceCount) == 1) {

        status = WdfFltrCreateControlDevice(Driver);
        if (!NT_SUCCESS(status)) {
            WdfFltrTrace(("WdfFltrCreateControlDevice failed - 0x%x\n", status));
            return status;
        }

    } else {

        WdfFltrTrace(("Control device already created, skipping\n"));

    }

    //
    // Success!
    //
    return STATUS_SUCCESS;

}



///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrCreateControlDevice
//
//      This is a helper routine used to create a control device object. We
//      will use this control device object to send data back to our user
//      mode component
//
//  INPUTS:
//
//      Driver - Address of our WDF driver object
//
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      An appropriate NTSTATUS code
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
//      Based heavily on techniques and code used in 
//      FilterCreateControlDevice in the WDF toaster filter sample
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrCreateControlDevice(
    IN WDFDRIVER Driver
    ) {

    PWDFDEVICE_INIT       deviceInit;
    WDF_IO_QUEUE_CONFIG   ioQueueConfig;
    NTSTATUS              status;
    WDF_FILEOBJECT_CONFIG fileObjectConfig;

    DECLARE_CONST_UNICODE_STRING(deviceName, WDFFLTR_CONTROL_NAME);
    DECLARE_CONST_UNICODE_STRING(linkName, WDFFLTR_CONTROL_LINK);

    if (WdfFltrControlDevice) {

        //
        // This can happen either in the free build or if
        //  the control device couldn't be deleted because
        //  a user application had an open handle to it
        //  when we last tried to delete it
        //
        WdfFltrTrace(("Control device already exists\n"));
        return STATUS_SUCCESS;

    }

    WdfFltrTrace(("Creating control device\n"));

    //
    // We need a PWDFDEVICE_INIT structure with which to
    //  create our WDFDEVICE. 
    //
    // Note that the SDDL string as the second parameter
    //  is required, the system will bugcheck without it.
    //  You can find pre-cooked SDDL strings in wdmsec.h 
    //  or make your own
    //
    deviceInit = WdfControlDeviceInitAllocate(
                            Driver,
                            &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RWX_RES_RWX);

    if (!deviceInit) {
        WdfFltrTrace(("WdfControlDeviceInitAllocate failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;

    }

    //
    // We want the control device exclusive, only one
    //  app should be talking to us
    //
    WdfDeviceInitSetExclusive(deviceInit, TRUE);

    //
    // Give it a name...
    //
    status = WdfDeviceInitAssignName(deviceInit, &deviceName);

    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfDeviceInitAssignName failed - 0x%x\n", status));
        return status;
    }

    //
    // We need to keep track of whether or not the user mode application
    //  has our control device open. Therfore, we'll need EvtCreate and
    //  EvtClose callbacks.
    //
    WDF_FILEOBJECT_CONFIG_INIT(&fileObjectConfig,
                               WdfFltrControlCreate,
                               WdfFltrControlClose,
                               WDF_NO_EVENT_CALLBACK);

    WdfDeviceInitSetFileObjectConfig(deviceInit,
                                     &fileObjectConfig,
                                     NULL);
    //
    // And create the control device
    //
    status = WdfDeviceCreate(&deviceInit,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &WdfFltrControlDevice);
    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfDeviceCreate failed - 0x%x\n", status));
        return status;
    }

    //
    // Create a symlink for the device so that the user mode
    //  app can open it.
    //
    status = WdfDeviceCreateSymbolicLink(WdfFltrControlDevice,
                                         &linkName);

    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfDeviceCreateSymbolicLink failed - 0x%x\n", status));
        return status;
    }

    //
    // Configure the default queue associated with the control device 
    //  object to be Serial so that request passed to EvtIoDeviceControl 
    //  are serialized.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                           WdfIoQueueDispatchSequential);

    //
    // And we'll only be needing device control requests. Note that 
    //  create/cleanup/close are all handled by the framework.
    //
    ioQueueConfig.EvtIoDeviceControl = WdfFltrAppIntDeviceControl;

    status = WdfIoQueueCreate(WdfFltrControlDevice,
                              &ioQueueConfig,
                              WDF_NO_OBJECT_ATTRIBUTES,
                              NULL);
    if (!NT_SUCCESS(status)) {
        WdfFltrTrace(("WdfIoQueueCreate failed - 0x%x\n", status));
        return status;
    }

    //
    // Control devices must notify WDF when they are done initializing.   
    //  I/O is rejected until this call is made.
    //
    WdfControlFinishInitializing(WdfFltrControlDevice);

    return STATUS_SUCCESS;

}


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrQueryRemove
//
//      This routine will be executed once for each of the WDFDEVICEs that we 
//      created when they are being asked if they can remove.
//
//      It is not a required routine, but we use it to clean up the control 
//      device object in the checked build (last one out deletes it)
//
//  INPUTS:
//
//      Device - Address of one of our WDF device objects
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      SUCCESS in the free build, may return STATUS_DEVICE_BUSY in the 
//      checked build if the user has the control device open.
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrQueryRemove(
    IN WDFDEVICE Device
    ) {

    if (InterlockedDecrement(&WdfFltrDeviceCount) == 0) {

#if DBG
        /////////////////////////////////////
        //                                 //
        // DEVELOPER NOTE:                 //
        //                                 ///////////////////////
        //                                                      //
        // AS OF WDF V1.0 THERE IS A BUG THAT MAKES THE SAFE    //
        // DELETION OF WDF CONTROL DEVICES IMPOSSIBLE! THE      //
        // ONLY SAFE OPTION IS NOT ALLOW A FILTER THAT CREATES  //
        // A CONTROL DEVICE TO BE UNLOADABLE. IN THE DEBUG      //
        // BUILD WE'LL TAKE THE RISK AND ALLOW THE CONTROL      //
        // DEVICE TO BE DELETED, BUT OTHERWISE NO CHANCE!       //
        //                                                      //
        //////////////////////////////////////////////////////////

        //
        // *** This check is entirely NOT thread safe, we could be
        //     in the middle of processing an open as we speak. 
        //     However, protecting this code against the EvtCreate
        //     callback only makes the BSOD window smaller and doesn't
        //     really close it, so we don't bother serializing and 
        //     only do this in the checked build (where an 
        //     occasional crash is OK)
        //
        if (WdfFltrControlDeviceOpen) {
            WdfFltrTrace(("Not allowing remove of device 0x%p, control "\
                          "device is open...\n", Device));
            //
            // The device didn't really go away...
            //
            InterlockedIncrement(&WdfFltrDeviceCount);
            return STATUS_DEVICE_BUSY;
        }

        DbgPrint("****Deleting Control Device, May Cause BSOD!\n");
        WdfObjectDelete(WdfFltrControlDevice);
#endif

    }

    return STATUS_SUCCESS;
}


/////////////////////////
// PNP/POWER CALLBACKS //
/////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrD0Entry
//
//      This routine is called when the device that we're filtering is
//      entering the D0 state (i.e. when it is powering up)
//
//  INPUTS:
//
//      Device - Address of one of our WDF device objects
//
//      PreviousState - The previous power state of the filtered device
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      STATUS_SUCCESS
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrD0Entry(
    IN WDFDEVICE Device,
    IN WDF_POWER_DEVICE_STATE PreviousState
    ) {

    WdfFltrTrace(("Power up from %s\n", WdfPowerStateToString(PreviousState)));

    return STATUS_SUCCESS;

}


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrD0Exit
//
//      This routine is called when the device that we're filtering is
//      leaving the D0 state (i.e. when it is powering down)
//
//  INPUTS:
//
//      Device - Address of one of our WDF device objects
//
//      PreviousState - The previous power state of the filtered device
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      STATUS_SUCCESS
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrD0Exit(
    IN WDFDEVICE Device,
    IN WDF_POWER_DEVICE_STATE PreviousState
    ) {

    WdfFltrTrace(("Power down from %s\n", WdfPowerStateToString(PreviousState)));

    return STATUS_SUCCESS;

}


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrWdmPnp
//    
//      WARNING: WDM CONTENT
//
//      This routine is is a WDM preprocessing callback for PnP requests.
//      Normally, a filter should not require this routine AT ALL. However,
//      it's nice in a filter to see the underlying PnP operations, since
//      you might be targeting a WDM device object with your filter (or 
//      because you're a WDM driver writer and want to see how the 
//      WDF callbacks map to the underlying WDM operations)
//
//  INPUTS:
//
//      Device - Address of one of our WDF device objects
//
//      Irp    - The IRP_MJ_PNP IRP.
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      STATUS_SUCCESS
//
//  IRQL:
//
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrWdmPnp(
    WDFDEVICE Device,
    PIRP Irp
    ) {

    WDFFLTR_REQUEST      requestEntry;
    PWDFFLTR_PNP_REQUEST pnpRequest;
    PIO_STACK_LOCATION   ioStack;

    //
    // Get the WDM I/O stack
    //
    ioStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // And spit out the info to the debugger.
    //
    WdfFltrTrace(("MinorFunction 0x%x\n", ioStack->MinorFunction));

    //
    // Also send info about the operation to user mode...
    //
    requestEntry.RequestType = WdfFltrPnPRequest;
    pnpRequest = &requestEntry.PnPRequest;
    pnpRequest->MinorFunction = ioStack->MinorFunction;
    WdfFltrAddRequestToTracker(&WdfFltrRequestBufferTracker,
                               &requestEntry);

    //
    // Just like in WDM, we need to setup the next stack location before 
    //  we pass the request on with WdfDeviceWdmDispatchPreprocessedIrp.
    //  We'll do that here by calling IoSkipCurrentIrpStackLocation 
    //
    IoSkipCurrentIrpStackLocation(Irp);

    //
    // Now give the request back to the framework with the special 
    //  WdfDeviceWdmDispatchPreprocessedIrp function
    //
    return WdfDeviceWdmDispatchPreprocessedIrp(Device,
                                               Irp);

}


///////////////////
// I/O CALLBACKS //
///////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrRead
//
//      This routine is called when the device that we're filtering is
//      sent a READ request
//
//  INPUTS:
//
//      Queue - Our filter device's default WDF queue
//
//      Request - The WDF read request
//
//      Length - The length of the read operation
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrRead(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t Length
    ) {

    WDFFLTR_REQUEST       requestEntry;
    PWDFFLTR_READ_REQUEST readRequest;
    PIRP                  wdmReadIrp;
    NTSTATUS              status;
    PCHAR                 dataBuffer = NULL;

    //
    // Retrieve some parameters of the request to print 
    //  to the debugger
    //
    if (Length != 0) {

        //
        // This is a non-zero length transfer, retrieve
        //  the data buffer.
        //
        status = WdfRequestRetrieveOutputBuffer(Request,
                                                Length,
                                                (PVOID *)&dataBuffer,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            //
            // Not good. We'll still pass the request down
            //  and let the next device decide what to do with 
            //  this.
            //
            WdfFltrTrace(("RetrieveOutputBuffer failed - 0x%x\n", 
                         status));
        }

    }

    //
    // Get the WDM IRP
    //
    wdmReadIrp = WdfRequestWdmGetIrp(Request);

    //
    // Print the info to the debugger
    //
    WdfFltrTrace(("IRP-0x%p; Buffer-0x%p; Length-0x%x\n",
        wdmReadIrp, dataBuffer, Length));


    //
    // And also send the information about the request to the user app
    //
    requestEntry.RequestType = WdfFltrReadRequest;
    readRequest = &requestEntry.ReadRequest;
    readRequest->BufferLength = Length;
    readRequest->IsCompletion = FALSE;
    WdfFltrAddRequestToTracker(&WdfFltrRequestBufferTracker,
                               &requestEntry);


    //
    // For reads, we want completion info. Call the helper
    //  routine to forward the request with a completion routine.
    //
    WdfFltrForwardRequestWithCompletion(WdfIoQueueGetDevice(Queue), 
                                        Request,
                                        WdfFltrReadComplete,
                                        NULL);
    return;

}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrReadComplete
//
//      This routine is our completion routine for read requests sent to the 
//      filtered device
//
//  INPUTS:
//
//      Queue   - Our filter device's default WDF queue
//
//      Target  - The I/O target of our default queue (i.e. an I/O target that
//                targets the filtered device
//
//      Params  - The completion information for the request
//
//      Context - Whatever context we supplied to WdfRequestSetCompletionRoutine
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrReadComplete(
    IN WDFREQUEST Request,
    IN WDFIOTARGET Target,
    IN PWDF_REQUEST_COMPLETION_PARAMS Params,
    IN WDFCONTEXT Context
    ) {

    WDFFLTR_REQUEST       requestEntry;
    PWDFFLTR_READ_REQUEST readRequest;
    PIRP                  wdmReadIrp;


    //
    // Get the IRP back
    //
    wdmReadIrp = WdfRequestWdmGetIrp(Request);

    //
    // And print the information to the debugger
    //
    WdfFltrTrace(("IRP-0x%p; Status-0x%x; Information-0x%x\n",
        wdmReadIrp, Params->IoStatus.Status, 
        Params->IoStatus.Information));

    //
    // Also send information about the completion to user mode.
    //
    requestEntry.RequestType = WdfFltrReadRequest;
    readRequest = &requestEntry.ReadRequest;
    readRequest->IsCompletion = TRUE;
    RtlCopyMemory(&readRequest->CompletionStatus,
                  &Params->IoStatus,
                  sizeof(IO_STATUS_BLOCK));
    WdfFltrAddRequestToTracker(&WdfFltrRequestBufferTracker,
                               &requestEntry);

    //
    // Restart completion processing.
    //
    WdfRequestComplete(Request, Params->IoStatus.Status);
    
}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrWrite
//
//      This routine is called when the device that we're filtering is
//      sent a WRITE request
//
//  INPUTS:
//
//      Queue - Our filter device's default WDF queue
//
//      Request - The WDF write request
//
//      Length - The length of the write operation
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrWrite(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t Length
    ) {

    WDFFLTR_REQUEST        requestEntry;
    PWDFFLTR_WRITE_REQUEST writeRequest;
    PIRP                   wdmWriteIrp;
    NTSTATUS               status;
    PCHAR                  dataBuffer = NULL;

    //
    // Retrieve some parameters of the request to print 
    //  to the debugger
    //
    if (Length != 0) {

        //
        // This is a non-zero length transfer, retrieve
        //  the data buffer.
        //
        status = WdfRequestRetrieveInputBuffer(Request,
                                               Length,
                                               (PVOID *)&dataBuffer,
                                               NULL);

        if (!NT_SUCCESS(status)) {

            //
            // Not good. We'll still pass the request down
            //  and let the next device decide what to do with 
            //  this.
            //
            WdfFltrTrace(("RetrieveInputBuffer failed - 0x%x\n", 
                         status));
        }

    }

    //
    // Get the WDM IRP
    //
    wdmWriteIrp = WdfRequestWdmGetIrp(Request);

    //
    // Print the info to the debugger
    //
    WdfFltrTrace(("IRP-0x%p; Buffer-0x%p; Length-0x%x\n",
        wdmWriteIrp, dataBuffer, Length));


    //
    // And also send the information about the request to the user app
    //
    requestEntry.RequestType = WdfFltrWriteRequest;
    writeRequest = &requestEntry.WriteRequest;
    writeRequest->BufferLength = Length;
    WdfFltrAddRequestToTracker(&WdfFltrRequestBufferTracker,
                               &requestEntry);


    //
    // For simplicity sake, we'll just send and forget writes
    //
    WdfFltrForwardRequest(WdfIoQueueGetDevice(Queue), 
                          Request);
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrDeviceControl
//
//      This routine is called when the device that we're filtering is
//      sent a DEVICE CONTROL request
//
//  INPUTS:
//
//      Queue              - Our filter device's default WDF queue
//
//      Request            - The WDF device control request
//
//      OutputBufferLength - Size of the output buffer, if any
//
//      InputBufferLength  - Size of the input buffer, if any
//
//      IoControlCode      - The operation
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrDeviceControl(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t OutputBufferLength,
    IN size_t InputBufferLength,
    IN ULONG IoControlCode
    ) {

    WDFFLTR_REQUEST                 requestEntry;
    PWDFFLTR_DEVICE_CONTROL_REQUEST deviceControlRequest;
    PIRP                            deviceControlIrp;

    //
    // Get the WDM IRP
    //
    deviceControlIrp = WdfRequestWdmGetIrp(Request);

    //
    // And print out some basic information. This routine would
    //  need to be customized based on the target device you're
    //  filtering.
    //
    WdfFltrTrace(("IRP-0x%p; InputBufferLength-0x%x; OutputBufferLength-0x%x; "\
                  "Control Code-0x%x\n",
                   deviceControlIrp, InputBufferLength, OutputBufferLength,
                   IoControlCode));

    //
    // Send the information to user mode.
    //
    requestEntry.RequestType = WdfFltrDeviceControlRequest;
    deviceControlRequest = &requestEntry.DeviceControlRequest;
    deviceControlRequest->InputBufferLength  = InputBufferLength;
    deviceControlRequest->OutputBufferLength = OutputBufferLength;
    deviceControlRequest->IoControlCode      = IoControlCode;
    WdfFltrAddRequestToTracker(&WdfFltrRequestBufferTracker,
                               &requestEntry);

    //
    // And pass the request on to the filtered device
    //
    WdfFltrForwardRequest(WdfIoQueueGetDevice(Queue), 
                          Request);
    return;

}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrDeviceControl
//
//      This routine is called when the device that we're filtering is
//      sent an INTERNAL DEVICE CONTROL request
//
//  INPUTS:
//
//      Queue              - Our filter device's default WDF queue
//
//      Request            - The WDF device control request
//
//      OutputBufferLength - Size of the output buffer, if any
//
//      InputBufferLength  - Size of the input buffer, if any
//
//      IoControlCode      - The operation
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrInternalDeviceControl(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t OutputBufferLength,
    IN size_t InputBufferLength,
    IN ULONG IoControlCode
    ) {

    WDFFLTR_REQUEST                 requestEntry;
    PWDFFLTR_DEVICE_CONTROL_REQUEST deviceControlRequest;
    PIRP                            deviceControlIrp;

    //
    // Get the WDM IRP
    //
    deviceControlIrp = WdfRequestWdmGetIrp(Request);

    //
    // And print out some basic information. This routine would
    //  need to be customized based on the target device you're
    //  filtering.
    //
    WdfFltrTrace(("IRP-0x%p; InputBufferLength-0x%x; OutputBufferLength-0x%x; "\
                  "Control Code-0x%x\n",
                   deviceControlIrp, InputBufferLength, OutputBufferLength,
                   IoControlCode));

    //
    // Send the information to user mode.
    //
    requestEntry.RequestType = WdfFltrInternalDeviceControlRequest;
    deviceControlRequest = &requestEntry.DeviceControlRequest;
    deviceControlRequest->InputBufferLength  = InputBufferLength;
    deviceControlRequest->OutputBufferLength = OutputBufferLength;
    deviceControlRequest->IoControlCode      = IoControlCode;
    WdfFltrAddRequestToTracker(&WdfFltrRequestBufferTracker,
                               &requestEntry);

    //
    // And pass the request on to the filtered device
    //
    WdfFltrForwardRequest(WdfIoQueueGetDevice(Queue), 
                          Request);
    return;

}


/////////////////////
// HELPER ROUTINES //
/////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrForwardRequest
//
//      There is a bit of annoying machination involved in passing a WDF
//      request, so this is just a helper routine to pass it on. 
//
//  INPUTS:
//
//      Queue              - On of our WDF filter devices
//
//      Request            - The WDF request to pass on
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrForwardRequest(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
    ) {

    WDF_REQUEST_SEND_OPTIONS options;
    PDEVICE_CONTEXT          devCont;
    NTSTATUS                 status;

    //
    // Get the context that we setup during DeviceAdd processing
    //
    devCont = WdfFltrGetDeviceContext(Device);

    ASSERT(IS_DEVICE_CONTEXT(devCont));

    //
    // We're just going to be passing this request on with 
    //  zero regard for what happens to it. Therefore, we'll
    //  use the WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET option
    //
    WDF_REQUEST_SEND_OPTIONS_INIT(
                        &options,
                        WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);


    //
    // And send it!
    // 
    if (!WdfRequestSend(Request, 
                        devCont->TargetToSendRequestsTo, 
                        &options)) {

        //
        // Oops! Something bad happened, complete the request
        //
        status = WdfRequestGetStatus(Request);
        WdfFltrTrace(("WdfRequestSend failed - 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrForwardRequestWithCompletion
//
//      There is a bit of annoying machination involved in passing a WDF
//      request, so this is just a helper routine to pass it on. The 
//      difference between this routine and WdfFltrForwardRequest 
//      is the ability to supply a completion routine and completion 
//      context.
//
//  INPUTS:
//
//      Queue              - On of our WDF filter devices
//
//      Request            - The WDF request to pass on
//
//      CompletionRoutine  - A completion routine for this request
//
//      CompletionContext  - A context pointer that will be passed
//                           back to the driver in the CompletionRoutine
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      None
//
//  IRQL:
//
//      Depends entirely on the device that you're filtering. Typically
//      <= DISPATCH_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrForwardRequestWithCompletion(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request,
    IN PFN_WDF_REQUEST_COMPLETION_ROUTINE CompletionRoutine,
    IN WDFCONTEXT CompletionContext
    ) {

    PDEVICE_CONTEXT devCont;
    NTSTATUS        status;

    //
    // Get the context that we setup during DeviceAdd processing
    //
    devCont = WdfFltrGetDeviceContext(Device);

    ASSERT(IS_DEVICE_CONTEXT(devCont));

    //
    // Setup the request for the next driver
    //
    WdfRequestFormatRequestUsingCurrentType(Request);

    //
    // Set the completion routine...
    //
    WdfRequestSetCompletionRoutine(Request,
                                   CompletionRoutine,
                                   CompletionContext);

    //
    // And send it!
    // 
    if (!WdfRequestSend(Request, 
                        devCont->TargetToSendRequestsTo, 
                        NULL)) {
        //
        // Oops! Something bad happened, complete the request
        //
        status = WdfRequestGetStatus(Request);
        WdfFltrTrace(("WdfRequestSend failed - 0x%x\n", status));
        WdfRequestComplete(Request, status);
    }
    return;
}



//
// Routine to get a string literal for a WDF_POWER_DEVICE_STATE 
//  value
//
PCHAR 
WdfPowerStateToString(
    IN WDF_POWER_DEVICE_STATE PowerState
    ) {

    switch (PowerState) {
    case WdfPowerDeviceInvalid:
        return "WdfPowerDeviceInvalid";
    case WdfPowerDeviceD0:
        return "WdfPowerDeviceD0";
    case WdfPowerDeviceD1:
        return "WdfPowerDeviceD1";
    case WdfPowerDeviceD2:
        return "WdfPowerDeviceD2";
    case WdfPowerDeviceD3:
        return "WdfPowerDeviceD3";
    case WdfPowerDeviceD3Final:
        return "WdfPowerDeviceD3Final";
    case WdfPowerDevicePrepareForHibernation:
        return "WdfPowerDevicePrepareForHibernation";
    case WdfPowerDeviceMaximum:
        return "WdfPowerDeviceMaximum";
    default:
        return "Unknown??";
    }
}
