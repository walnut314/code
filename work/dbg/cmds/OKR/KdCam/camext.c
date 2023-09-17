#include "camext.h"

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
    dprintf("\nCopyright © 2023 Intel Corp. All rights reserved.\n\n");
    dprintf("help       - Displays this list.\n");
    dprintf("mem        - Dumps the used_mem_list\n");
}

DECLARE_API( version )
{
    dprintf( "Camera extension dll for Camera Build %d debugging %s kernel for Kernel Build %d\n",
             CameraVersion,
             SavedMajorVersion == 0x0c ? "Checked" : "Free",
             SavedMinorVersion
           );
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

//#include <framecontext.h>
//#include <isp_frame.h>
DECLARE_API(frames)
{
    ULONG64 Count;
    ULONG64 Frames;
    ULONG32 n_Frames;
    int Size;
    DWORD dwBytesRead;
    //_DrvFrameContext frame_ctx;
    int i;

    Count  = GetExpression("iacamera64!g_MaxFramesToBeQueued");
    Frames = GetExpression("iacamera64!g_frameContexts");
    //Size   = GetExpression("?? sizeof(iacamera64!_DrvFrameContext)");
    //Size   = GetExpression("sizeof(iacamera64!_DrvFrameContext)");
    Size   = 0xdeadbeef; // Exec("sizeof(iacamera64!g_frameContexts)");
    dprintf("Count: %I64x, Frames: %I64x, Size: %I64d\n", Count, Frames, Size);

    ReadMemory(Count, &n_Frames, sizeof( ULONG32 ), &dwBytesRead);
    dprintf("num frames: %d\n", n_Frames);
    //size = sizeof(_DrvFrameContext);
    for (i = 0; i < n_Frames; i++) {

    }
#if 0
    dprintf("n frames %d\n", n_Frames);

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
#endif
}


