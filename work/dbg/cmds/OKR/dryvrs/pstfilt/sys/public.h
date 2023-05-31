/*++
--*/

//
// Device type           -- in the "User Defined" range."
//
#define FILEIO_TYPE 40001
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
#define IOCTL_PST_METHOD_FAIL_STACK \
    CTL_CODE( FILEIO_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS  )

#define IOCTL_PST_METHOD_RESTORE_STACK \
    CTL_CODE( FILEIO_TYPE, 0x901, METHOD_BUFFERED , FILE_ANY_ACCESS  )


#define DRIVER_FUNC_INSTALL     0x01
#define DRIVER_FUNC_REMOVE      0x02

#define DRIVER_NAME       "PST"
#define DEVICE_NAME       "\\\\.\\PST\\pstsamp.log"
