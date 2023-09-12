#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <stddef.h>

typedef unsigned long long PMDL;

#if 0
#include "..\..\ISP\hrt2\inc\shared_memory_access_types.h"
#include "..\..\ISP\hrt2\memory_range.h"
#endif

typedef unsigned long long host_virtual_address_t;
typedef unsigned long      vied_virtual_address_t;

typedef struct _memory_range memory_range_t, *pmem_range;
struct _memory_range {
    LIST_ENTRY list;
    host_virtual_address_t host_addr;          // VTL0 insecure buffer kernel virtual address
    vied_virtual_address_t vied_addr;          // Buffer device virtual address
    HANDLE                 dvmm_node;          // Non-vsm mode only, for DVMM management
    HANDLE                 vtl0_buffer_handle; // VSM mode only, for VTL0 insecure buffer
    HANDLE                 vtl1_secure_handle; // VSM mode only, for VTL1 secure section buffer handle
    GUID                   vtl1_buffer_guid;   // VSM mode only, for VTL1 secure buffer
    PMDL page_addr;                            // VTL0 inscure buffer mdl
    size_t actual_size;                        // Buffer size
};


#endif
