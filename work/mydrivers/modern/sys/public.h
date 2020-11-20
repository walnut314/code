/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.


Module Name:

    PUBLIC.H

Abstract:


    Defines the IOCTL codes that will be used by this driver.  The IOCTL code
    contains a command identifier, plus other information about the device,
    the type of access with which the file must have been opened,
    and the type of buffering.

Environment:

    Kernel mode only.

--*/

//
// Device type           -- in the "User Defined" range."
//
#define FILEIO_TYPE 40001
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
#define IOCTL_MODERN_START_THREAD \
    CTL_CODE( FILEIO_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS  )

#define IOCTL_MODERN_STOP_THREAD \
    CTL_CODE( FILEIO_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS  )

#define IOCTL_MODERN_QUEUE_REQUEST \
    CTL_CODE( FILEIO_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS  )


#define DRIVER_FUNC_INSTALL     0x01
#define DRIVER_FUNC_REMOVE      0x02

#define DRIVER_NAME       "MODERN"
#define DEVICE_NAME       "\\\\.\\Modern"

typedef struct _SWITCH_STATE {
    BOOLEAN State;
} SWITCH_STATE, *PSWITCH_STATE;

