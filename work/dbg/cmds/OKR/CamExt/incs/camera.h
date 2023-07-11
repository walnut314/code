#ifndef _CAMERA_H_
#define _CAMERA_H_

typedef struct _CAM {
    ULONG dude1;
    ULONG dude2;
    ULONG dude3;
    ULONG dude4;

} CAM, *PCAM;

typedef struct _NT_IMAGE_INFO {
   ULONG Version;
   ULONG OsMajorVersion;
   ULONG OsMinorVersion;
   ULONG MajorRelease;
   ULONG LoaderBlockSize;
   ULONG LoaderExtensionSize;
} NT_IMAGE_INFO, *PNT_IMAGE_INFO;

typedef struct _KPROCESS {
   ULONG64 Header;
   LIST_ENTRY ProcessListEntry;
   LIST_ENTRY ProfileListHead;
   ULONG64 DirectoryTableBase;
   LIST_ENTRY ThreadListHead;
   ULONG ProcessLock;
} KPROCESS, *PKPROCESS;

//#undef _MDL
//#undef PMDL
typedef unsigned long long PMDL;

#include "..\..\ISP\hrt2\inc\shared_memory_access_types.h"
#include "..\..\ISP\hrt2\memory_range.h"

#if 0
typedef unsigned long long host_virtual_address_t;
typedef unsigned long long vied_virtual_address_t;
typedef unsigned long long PMDL;

// following from: Camera\ISP\hrt2\memory_range.h
typedef struct _memory_range memory_range_t, *pmem_range;
struct _memory_range {
    LIST_ENTRY list;
    host_virtual_address_t host_addr;          // VTL0 insecure buffer kernel virtual address
    vied_virtual_address_t vied_addr;          // Buffer device virtual address
    HANDLE                 dvmm_node;          // Non-vsm mode only, for DVMM management
    HANDLE                 vtl0_buffer_handle; // VSM mode only, for VTL0 insecure buffer
    GUID                   vtl1_buffer_guid;   // VSM mode only, for VTL1 secure buffer
    PMDL                   page_addr;          // VTL0 inscure buffer mdl
    ULONG                  status;
    size_t                 actual_size;        // Buffer size
};
#endif


#endif
