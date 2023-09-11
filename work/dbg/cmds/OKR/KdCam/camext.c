#include <ntddk.h>
#include <windef.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000

#define LPTR (LMEM_FIXED | LMEM_ZEROINIT)

#define WINBASEAPI
    
WINBASEAPI
HLOCAL
WINAPI
LocalAlloc(
    UINT uFlags,
    UINT uBytes
    );

WINBASEAPI
HLOCAL
WINAPI
LocalFree(
    HLOCAL hMem
    );

#define CopyMemory RtlCopyMemory
#define FillMemory RtlFillMemory
#define ZeroMemory RtlZeroMemory

//
// Now we can bring in the WINDBG extension
// definitions...
//
#include <wdbgexts.h>   // located in MSTOOLS\H

#include <shared_memory_access.h>
#include <shared_memory_access_priv.h>
#include <memory_range.h>

static EXT_API_VERSION
    ApiVersion = { 5, 1, EXT_API_VERSION_NUMBER64, 0 };

static WINDBG_EXTENSION_APIS ExtensionApis;

static USHORT SavedMajorVersion;
static USHORT SavedMinorVersion;

static USHORT CameraVersion = 13489;

LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
    )
{
    return &ApiVersion;
}

VOID
CheckVersion(
    VOID
    )
{
    //
    // Your version-checking code goes here
    //
    dprintf( 
        "CheckVersion called... [%1x;%d]\n",
        SavedMajorVersion,
        SavedMinorVersion
        );
}

VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT MajorVersion,
    USHORT MinorVersion
    )
{
    //
    // Save pointer to the table of utility
    // functions provided by WINDBG
    //
    // Functions defined in WDBGEXTS.H assume
    // that the saved pointer has exactly this
    // name. Don't change it !
    //
    ExtensionApis = *lpExtensionApis;

    //
    // NOTE: For a list of the helper functions
    // and descriptions of what they do, see the
    // online help in WINDBG itself. From the
    // Contents screen, click on the KD button,
    // then click on the "Creating Extensions"
    // topic. Scroll about half way down this
    // topic and you'll find a list of helper
    // functions. Click on the name of a function
    // to see its prototype and a description.
    //

    //
    // Save information about the version of
    // NT that's being debugged
    //
    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;

    return;
}

DECLARE_API(help)
{
    dprintf("mem -- dumps the used_mem_list\n");
}

DECLARE_API(mem)
{
    ULONG64 Address;
    DWORD dwBytesRead;
    ULONG64 Flink;
    ULONG64 Blink;
    LIST_ENTRY list;
    memory_range_t mem;
    PCHAR cmd[256];
    size_t full_len = 256;
    size_t total_size = 0;

    Address = GetExpression("iacamera64!used_mem_list");
    dprintf("list %I64x\n", Address);

    if( !ReadMemory(Address, &list, sizeof( LIST_ENTRY ), &dwBytesRead))
    {
        dprintf( "Can't get list.\n ");
        return;
    }

    dprintf("flink: %I64x\n", list.Flink);
    Flink = (ULONG64) list.Flink;
    Blink = (ULONG64) list.Blink;

    while (Flink != Blink) {
        ReadMemory(Flink, &mem, sizeof(memory_range_t), &dwBytesRead);
        dprintf("host addr %I64x\n", mem.host_addr);
        dprintf("mdl       %I64x\n", mem.page_addr);
        dprintf("size      0n%d\n", mem.actual_size);
        total_size += mem.actual_size;
        Flink = (ULONG64) mem.list.Flink;
    }
    dprintf("total size 0n%d bytes, 0n%d meg \n", total_size, total_size/1024/1024);

}

DECLARE_API( version )
{
    dprintf( "Camera extension dll for Camera Build %d debugging %s kernel for Kernel Build %d\n",
             CameraVersion,
             SavedMajorVersion == 0x0c ? "Checked" : "Free",
             SavedMinorVersion
           );
}


