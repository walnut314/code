#include <stdio.h>
#include "ntdef.h"
#include "genkfilt_ioctl.h"
//
//
HANDLE connect_start_event();

//int _cdecl main (int argc, char *argv[])
//{
//    HANDLE hStartEvent;
//    hStartEvent = connect_start_event();
//    return 0;
//}

HANDLE connect_start_event()
{        
    NTSTATUS status;
    HANDLE kevent = NULL;
    UNICODE_STRING uString;
    OBJECT_ATTRIBUTES objattr;
    ULONG res;

    RtlInitUnicodeString(&uString, GENKFILT_EVENT_KERNEL_NAME);
    InitializeObjectAttributes(&objattr, &uString, 0, NULL, NULL);
    status = NtOpenEvent(&kevent, SYNCHRONIZE/*EVENT_ALL_ACCESS*/, &objattr);
    if (!NT_SUCCESS(status)) {
        printf("failed to open event %x\n", status);
        goto error_exit;
    }

    printf("waiting for event \n");
    while (1) {
        printf("waiting for event \n");
        res = WaitForSingleObject(kevent, INFINITE);
        switch (res) {
            case WAIT_ABANDONED:
                printf("wait abandoned\n");
                goto error_exit;
                break;
            case WAIT_OBJECT_0:
                printf("wait succeeded\n");
                break;
            case WAIT_TIMEOUT:
                printf("wait timeout\n");
                goto error_exit;
                break;
            default:
                break;
        }
    }

error_exit:    
    return kevent;
    
}


