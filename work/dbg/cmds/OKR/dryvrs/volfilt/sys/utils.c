#include <wdm.h>



BOOL
PDRIVE_LAYOUT_INFORMATION_EX
GetDriveLayoutEx(PDEVICE_OBJECT DeviceObject)
{
    BOOL status;
    HANDLE device;
    DISK_GEOMETRY geometry;
    PDRIVE_LAYOUT_INFORMATION_EX p_drive_layout_ex = NULL;
    PPARTITION_INFORMATION_EX p_part_info_ex = NULL;
    ULONG returnedBytes, buf_size = 16 * sizeof(DRIVE_LAYOUT_INFORMATION_EX);
    ULONG i;
    unsigned char *buffer = NULL;

    //printf("get drive layout %s\n", dev_path);

    while (1) {
        p_drive_layout_ex = malloc(buf_size);
        if (!p_drive_layout_ex) {
            CloseHandle(device);
            return FALSE;
        }
        status = DeviceIoControl(device,
                                 IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
                                 NULL,
                                 0,
                                 p_drive_layout_ex,
                                 buf_size,
                                 &returnedBytes,
                                 FALSE);
        
        if (!status) {
            errno = GetLastError();
            //printf("Error getting drive layout %d, returned %d\n", GetLastError(), returnedBytes);
            if (errno == ERROR_INSUFFICIENT_BUFFER) {
                free(p_drive_layout_ex);
                buf_size += 8 * sizeof(DRIVE_LAYOUT_INFORMATION);
                continue;
            }
            printf("Error getting drive layout %d, returned %d\n", errno, returnedBytes);
            free(p_drive_layout_ex);
            p_drive_layout_ex = NULL;
            break;
        } else {
            break;
        }
    }

#if 1
    if (status) {
        printf("drive layout ex:\n");
        printf("------------------\n");
        //printf("  PartitionStyle : 0x%x\n", p_drive_layout_ex->PartitionStyle);
        printf("  PartitionCount :   %d\n", p_drive_layout_ex->PartitionCount);
        //printf("  Signature      : 0x%x\n", p_drive_layout_ex->Signature);
        for (i = 0; i < p_drive_layout_ex->PartitionCount; i++) {
            printf("\n  PARTITION #%d\n", i);
            p_part_info_ex = &p_drive_layout_ex->PartitionEntry[i];
            //if (p_part_info->RecognizedPartition) {
            //printf("     Start     0x%I64x : Len %I64d : Style %x\n", 
            //                    p_part_info_ex->StartingOffset.QuadPart,
            //                    p_part_info_ex->PartitionLength.QuadPart,
            //                    p_part_info_ex->PartitionStyle);
            //}
            //printf("     Hidden sectors       %d\n", p_part_info_ex->HiddenSectors);
            //printf("     Partition number     %d\n", p_part_info_ex->PartitionNumber);
            //printf("     Boot indicator       %d\n", p_part_info_ex->BootIndicator);
            //printf("     Rewrite partition    %d\n\n", p_part_info_ex->RewritePartition);
            switch (p_part_info_ex->PartitionStyle) {
                case PARTITION_STYLE_MBR:
                    printf("    MBR is recognized? %d\n", p_part_info_ex->Mbr.RecognizedPartition);
                    printf("      Partition number     %d\n", p_part_info_ex->PartitionNumber);
                    printf("      Start     0x%I64x : Len %I64d\n", 
                                        p_part_info_ex->StartingOffset.QuadPart,
                                        p_part_info_ex->PartitionLength.QuadPart);
                    break;
                case PARTITION_STYLE_GPT:
                    printf("    GPT\n");
                    break;
                case PARTITION_STYLE_RAW:
                    printf("    RAW\n");
                    break;

            }
        }
        printf("\n");
    }
#endif
    CloseHandle(device);
    return (TRUE);
}

