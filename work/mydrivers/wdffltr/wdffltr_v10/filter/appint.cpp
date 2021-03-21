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
//      appintf.cpp
//
//    ABSTRACT:
//
//      This file contains the all of the code in our WDF filter that 
//      interfaces with the user mode component
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


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrControlCreate
//
//      This routine is the callback for opens to the control device.
//
//  INPUTS:
//
//      Device     - The control device
//  
//      Request    - The create/open request
//
//      FileObject - The representation of the open instance to the control
//                   device
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
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrControlCreate(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request,
    IN WDFFILEOBJECT FileObject
    ) {

    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(FileObject);
    //WdfFltrTrace(("Control device being opened\n"));

    //
    // Globally set the device as open
    //
    WdfFltrControlDeviceOpen = TRUE;

    WdfRequestComplete(Request, STATUS_SUCCESS);

    return;
}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrControlClose
//
//      This routine is the callback for closes of the control device.
//
//  INPUTS:
//
//      FileObject - The representation of the open instance to the control
//                   device given to us in WdfFltrControlCreate
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
//      PASSIVE_LEVEL.
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrControlClose(
    IN WDFFILEOBJECT FileObject
    ) {
    
    UNREFERENCED_PARAMETER(FileObject);
    //WdfFltrTrace(("Control device being closed\n"));
    //
    // Globally set the device as closed
    //
    WdfFltrControlDeviceOpen = FALSE;
    return;
}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrInitRequestBufferTracker
//
//      This routine initializes a WDFFLTR_REQUEST_BUFFER_TRACKER structure
//
//  INPUTS:
//
//      BufferTracker - The structure to initialize
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
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrInitRequestBufferTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker
    ) {


    //
    // Intializae the list of WDFFLTR_REQUEST_BUFFERs in the tracker 
    //  structure. Once the "current" buffer fills up it will be 
    //  put on this list to be picked up but the user mode app
    //
    InitializeListHead(&BufferTracker->RequestBuffers);
    
    KeInitializeSpinLock(&BufferTracker->RequestBuffersLock);

    BufferTracker->Tracking = FALSE;

    //
    // Allocate the active buffer. We'll always have at least
    //  one of these around.
    //
    BufferTracker->ActiveBuffer 
                = (PWDFFLTR_REQUEST_BUFFER)
                         ExAllocatePoolWithTag(NonPagedPool,
                                               sizeof(WDFFLTR_REQUEST_BUFFER),
                                               'BRsO');

    if (!BufferTracker->ActiveBuffer) {
        //WdfFltrTrace(("ExAllocatePoolWithTag failed\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The active buffer is empty
    //
    BufferTracker->ActiveBuffer->ValidRequestCount = 0;

    return STATUS_SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrStartRequestBufferTracker
//
//      This routine starts the logging in the specified buffer tracker
//
//  INPUTS:
//
//      BufferTracker - The structure to start
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
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrStartRequestBufferTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker
    ) {

    //
    // Just flag it as started...
    //
    BufferTracker->Tracking = TRUE;

    return STATUS_SUCCESS;

}


///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrStopRequestBufferTracker
//
//      This routine stops the logging in the specified buffer tracker.
//      This includes cleaning up any outstanding request buffers that
//      haven't been picked up by user mode
//
//  INPUTS:
//
//      BufferTracker - The structure to stop
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
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrStopRequestBufferTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker
    ) {
    
    KIRQL oldIrql;

    WdfFltrLockBufferTracker(BufferTracker, &oldIrql);

    BufferTracker->Tracking = FALSE;

    while (!IsListEmpty(&BufferTracker->RequestBuffers)) {

        ExFreePool(RemoveHeadList(&BufferTracker->RequestBuffers));

    }

    BufferTracker->PendingRequestBuffersCount = 0;

    BufferTracker->ActiveBuffer->ValidRequestCount = 0;

    WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);

    return STATUS_SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrAddRequestToTracker
//
//      This routine adds the specified WDFFTLR_REQUEST to the buffer tracker.
//
//  INPUTS:
//
//      BufferTracker - The buffer tracker
//
//      WdfFltrRequest - A description of a WDFREQUEST received by the filter
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
WdfFltrAddRequestToTracker(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker,
    IN PWDFFLTR_REQUEST WdfFltrRequest
    ) {

    KIRQL                   oldIrql;
    PWDFFLTR_REQUEST_BUFFER activeBuffer;
    PWDFFLTR_REQUEST_BUFFER newBuffer;

    WdfFltrLockBufferTracker(BufferTracker, &oldIrql);

    //
    // Not doing any tracking? Get out...
    //
    if (!BufferTracker->Tracking) {
        WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);
        return;
    }

    activeBuffer = BufferTracker->ActiveBuffer;

    //
    // Copy the structure to the tail of the WdfRequests array
    //  in the active buffer
    //
    RtlCopyMemory(&activeBuffer->WdfRequests[activeBuffer->ValidRequestCount++],
                  WdfFltrRequest,
                  sizeof(WDFFLTR_REQUEST));

    //
    // Did we fill it up?
    //
    if (activeBuffer->ValidRequestCount == WDFFLTR_REQUESTS_PER_BUFFER) {

        //
        // Have we filled up too many?
        //
        if (BufferTracker->PendingRequestBuffersCount 
                                == WDFFLTR_MAX_PENDING_BUFFERS) {

            //WdfFltrTrace(("Too many pending buffers...Data loss imminent\n"));

            activeBuffer->ValidRequestCount = 0;
            WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);
            return;

        }

        //
        // We haven't passed the acceptable threshold of active buffer
        //  requests but we've filled up the current one (app must be
        //  running a bit slow). Allocate a new buffer..
        // 
        newBuffer = (PWDFFLTR_REQUEST_BUFFER)
                          ExAllocatePoolWithTag(NonPagedPool,
                                                sizeof(WDFFLTR_REQUEST_BUFFER),
                                                '1RsO');
        if (!newBuffer) {

            //WdfFltrTrace(("Bad news...ExAllocatePoolWithTag failed\n"));

            //
            // Just lose everything that we've collected...System is
            //  going to die anyway...
            //
            activeBuffer->ValidRequestCount = 0;
            WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);
            return;

        }

        //
        // Time to serve this one up for consumption and move
        //  onto the next buffer.
        //
        InsertTailList(&BufferTracker->RequestBuffers,
                       &activeBuffer->ListEntry);

        BufferTracker->PendingRequestBuffersCount++;

        BufferTracker->ActiveBuffer = newBuffer;
        BufferTracker->ActiveBuffer->ValidRequestCount = 0;

    }

    WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);

    return;

}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrConsumeRequestBuffer
//
//      This routine pulls a request buffer, if any, out of the buffer tracker
//
//  INPUTS:
//
//      BufferTracker - The buffer tracker
//
//  OUTPUTS:
//
//      RequestBuffer - A partially or fully filled WDFFLTR_REQUEST_BUFFER
//                      is copied into this buffer
//
//  RETURNS:
//
//      An appropriate NTSTATUS code
//
//  IRQL:
//
//      PASSIVE_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
NTSTATUS
WdfFltrConsumeRequestBuffer(
    IN PWDFFLTR_REQUEST_BUFFER_TRACKER BufferTracker,
    OUT PWDFFLTR_REQUEST_BUFFER RequestBuffer
    ) {

    KIRQL                   oldIrql;
    PWDFFLTR_REQUEST_BUFFER bufferToCopy;

    WdfFltrLockBufferTracker(BufferTracker, &oldIrql);

    //
    // Any pending requests?
    //
    if (BufferTracker->PendingRequestBuffersCount) {

        //
        // Yup. Remove an entry from the header...
        //
        bufferToCopy 
            = (PWDFFLTR_REQUEST_BUFFER)
                        RemoveHeadList(&BufferTracker->RequestBuffers);

        //
        // Sanity check....
        //
        ASSERT(bufferToCopy->ValidRequestCount);

        //
        // One less request
        //
        BufferTracker->PendingRequestBuffersCount--;

        //
        // Copy the data...
        //
        RtlCopyMemory(RequestBuffer,
                      bufferToCopy,
                      sizeof(WDFFLTR_REQUEST_BUFFER));

        WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);

        //
        // Free the consumed buffer...
        //
        ExFreePool(bufferToCopy);
        
        return STATUS_SUCCESS;

    } else {

        //
        // OK, no pending buffers, but maybe our active
        //  buffer has some requests in it...
        //
        bufferToCopy = BufferTracker->ActiveBuffer;

        if (bufferToCopy->ValidRequestCount) {

            //
            // It does, copy everything out of the active buffer
            //
            RtlCopyMemory(RequestBuffer,
                          bufferToCopy,
                          sizeof(WDFFLTR_REQUEST_BUFFER));

            //
            // And reset the count...
            //
            bufferToCopy->ValidRequestCount = 0;

            WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);
            return STATUS_SUCCESS;

        }

    }

    WdfFltrUnLockBufferTracker(BufferTracker, oldIrql);
    //
    // Nothing to copy...
    //
    return STATUS_END_OF_FILE;

}

///////////////////////////////////////////////////////////////////////////////
//
//  WdfFltrAppIntDeviceControl
//
//      This routine is the EvtIoDeviceControl callback for our control device.
//
//  INPUTS:
//
//      Queue - The default queue for our control device
//
//      Request - A device control request from our user mode app
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
//      PASSIVE_LEVEL
//
//  NOTES:
//
///////////////////////////////////////////////////////////////////////////////
VOID
WdfFltrAppIntDeviceControl(
    IN WDFQUEUE Queue,
    IN WDFREQUEST Request,
    IN size_t OutputBufferLength,
    IN size_t InputBufferLength,
    IN ULONG IoControlCode
    ) {

    PWDFFLTR_REQUEST_BUFFER requestBuffer;
    NTSTATUS                status;
    size_t                  realLength;
    size_t                  bytesRemaining;

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    //
    // Let's see what we have...
    //
    switch (IoControlCode) {

    case IOCTL_WDFFLT_START_TRACK: {

        //
        // We only have a global tracker, start that...
        //
        (VOID)WdfFltrStartRequestBufferTracker(&WdfFltrRequestBufferTracker);
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return;

    }
    case IOCTL_WDFFLT_STOP_TRACK: {
        
        //
        // We only have a global tracker, stop that...The subroutine
        //  will deal with an already stopped tracker, but we won't
        //  bother with failing the request
        //
        (VOID)WdfFltrStopRequestBufferTracker(&WdfFltrRequestBufferTracker);
        WdfRequestComplete(Request, STATUS_SUCCESS);
        return;

    }

    case IOCTL_WDFFLT_GET_BUFFER: {

        //
        // IOCTL_WDFFLT_GET_BUFFER is where the payoff occurs, it's
        //  the user mode app calling us to see if we have any data.
        //  For our simple filter the user mode app will poll and we'll
        //  only return data if we have it....
        //
        if (InputBufferLength) {

            //WdfFltrTrace(("Sorry buddy...No input buffers allowed\n"));
            WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
            return;

        }

        //
        // Get the output buffer...
        //
        status = WdfRequestRetrieveOutputBuffer(Request,
                                                sizeof(WDFFLTR_REQUEST_BUFFER),
                                                (PVOID *)&requestBuffer,
                                                &realLength);

        if (!NT_SUCCESS(status)) {
            //WdfFltrTrace(("WdfRequestRetrieveOutputBuffer failed - 0x%x\n",
            //    status));
            WdfRequestComplete(Request, status);
            return;
        }
      
        //
        // And loop until we've run out of request buffers or we've
        //  exhausted the user mode buffer...
        //
        bytesRemaining = realLength;

        do {
    
            status = WdfFltrConsumeRequestBuffer(&WdfFltrRequestBufferTracker,
                                                 requestBuffer);

            if (!NT_SUCCESS(status)) {
                //
                // Could be because something bad happened our because
                //  we've already consumed everything there is to be 
                //  consumed...Break...
                //
                break;
            }

            //
            // We've used some of the data buffer..
            //
            bytesRemaining -= sizeof(WDFFLTR_REQUEST_BUFFER);

            //
            // And move up into the user's buffer...
            //
            requestBuffer++;

        } while (bytesRemaining >= sizeof(WDFFLTR_REQUEST_BUFFER));

        //
        // Did we copy anything?
        //
        if (bytesRemaining != realLength) {
   
            //
            // Yes! Return to the user, telling them how many bytes
            //  we copied....
            //
            WdfRequestCompleteWithInformation(Request, 
                                              STATUS_SUCCESS,
                                              realLength - bytesRemaining);

        } else {
            
            //
            // Nope, fail it...
            //
            WdfRequestComplete(Request, 
                               status);

        }
        return;
    }
    default:
        break;
    }

    WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);


    return;

}



