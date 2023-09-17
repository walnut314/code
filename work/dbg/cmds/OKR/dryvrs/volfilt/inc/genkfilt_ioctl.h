#if !defined(_IOCTL_H_)
#define _IOCTL_H_

#define IOCTL_GENKFILT_BASE             'k'
#define IOCTL_GENKFILT_QUERY_SETTINGS  CTL_CODE(IOCTL_GENKFILT_BASE, 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GENKFILT_SET             CTL_CODE(IOCTL_GENKFILT_BASE, 0x0001, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GENKFILT_RESET           CTL_CODE(IOCTL_GENKFILT_BASE, 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)

//#define GENKFILT_EVENT_KERNEL_NAME L"\\GenKFiltEvent"
//#define GENKFILT_EVENT_KERNEL_NAME L"GenKFiltEvent"
//#define GENKFILT_EVENT_KERNEL_NAME L"\\BaseNamedObjects\\GenKFiltEvent"
#define GENKFILT_EVENT_KERNEL_NAME L"\\Device\\GenKFiltEvent"
#define GENKFILT_EVENT_LINK_NAME   L"\\DosDevices\\GenKFiltEvent"
#define GENKFILT_EVENT_USER_NAME   "\\\\.\\GenKFiltEvent"

typedef struct _GENK_DEV {
    ULONG     DeviceType;
    ULONG     SerialNo;
    ULONG     DiskNumber;
    BOOLEAN   Blocked;    
} GENK_DEV, *PGENK_DEV;

typedef struct _QUERY_SETTINGS {
    int NumItems;
    GENK_DEV dev[1];
} QUERY_SETTINGS, *PQUERY_SETTINGS;

#endif

