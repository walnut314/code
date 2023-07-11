/*++

--*/

#include "stdafx.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PDEBUG_CONTROL4 DebugControl;
static PDEBUG_SYMBOLS3 DebugSymbols;
static PDEBUG_DATA_SPACES4 DebugDataSpaces;

static
HRESULT
QueryInterfaces(
    _In_ PDEBUG_CLIENT DebugClient
    )
{
    HRESULT Status = S_OK;

    if ((Status = DebugClient->QueryInterface(__uuidof(IDebugControl), (PVOID *)&DebugControl)) != S_OK) {

        return Status;
    }

    if ((Status = DebugClient->QueryInterface(__uuidof(IDebugSymbols), (PVOID *)&DebugSymbols)) != S_OK) {

        return Status;
    }

    if ((Status = DebugClient->QueryInterface(__uuidof(IDebugDataSpaces), (PVOID *)&DebugDataSpaces)) != S_OK) {

        return Status;
    }

    return Status;
}

static
VOID
ReleaseInterfaces(
    VOID
    )
{
    if (DebugDataSpaces) {

        DebugDataSpaces->Release();
        DebugDataSpaces = NULL;
    }

    if (DebugSymbols) {

        DebugSymbols->Release();
        DebugSymbols = NULL;
    }

    if (DebugControl) {

        DebugControl->Release();
        DebugControl = NULL;
    }
}

HRESULT
CALLBACK
mem_used(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    HRESULT Status = S_OK;
    BOOL IsAll = FALSE;
    ULONG BytesRead;
    ULONG64 Info;
    LIST_ENTRY list;
    ULONG64 listAddr;
    char cmd[] = "!list -t nt!_LIST_ENTRY.Flink -e -x \"dt iacamera64!_memory_range @$extret host_addr page_addr actual_size\" \0";

    __try {
        if ((Status = QueryInterfaces(DebugClient)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Could not query interfaces.\n");
            __leave;
        }
        if ((Status = IsKernelMode(DebugClient, __FUNCTION__)) != S_OK) {
            __leave;
        }

        if (args) {
            if ((args[0] == '-' || args[0] == '/') && args[1] == 'a') {
                IsAll = TRUE;
            }
        } else { // args is never NULL
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "!cam1 usage: wusup\n");
            return Status;
        }

        Status = DebugControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, "x iacamera64!used_mem_list", DEBUG_EXECUTE_ECHO);

        Status = DebugSymbols->GetOffsetByName("iacamera64!used_mem_list", &Info);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "list: %I64x\n", Info);
        Status = DebugDataSpaces->ReadVirtual(Info, &list, sizeof(LIST_ENTRY), &BytesRead);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "flink: %I64x\n", list.Flink);
        size_t full_len = strlen(cmd) + 4*sizeof(ULONG64);
        char* full_cmd = new char[full_len];
        listAddr = (ULONG64) list.Flink;
        sprintf_s(full_cmd, full_len, "%s %I64x", cmd, listAddr);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "yoyo: %d len %d %s\n", full_len, strlen(full_cmd), full_cmd);
        Status = DebugControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, full_cmd, DEBUG_EXECUTE_ECHO);

    }


    __finally {
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: BAM!!!\n");
        ReleaseInterfaces();
    }
    return Status;
}

HRESULT
CALLBACK
cam1(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    HRESULT Status = S_OK;
    BOOL IsAll = FALSE;
    ULONG64 CamMemList = 0xffffe58b446cc5e0;
    ULONG BytesRead;
    ULONG64 Info;
    ULONG64 ProcListAddr;
    CAM camera;
    NT_IMAGE_INFO ntInfo;
    LIST_ENTRY list;
    ULONG64 listAddr;
    ULONG64 KProcAddr;
    KPROCESS KProc;
    char cmd[] = "!list -t nt!_LIST_ENTRY.Flink -e -x \"dt iacamera64!_memory_range @$extret host_addr page_addr actual_size\" \0";
    int i;

    __try {
        if ((Status = QueryInterfaces(DebugClient)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Could not query interfaces.\n");
            __leave;
        }
        if ((Status = IsKernelMode(DebugClient, __FUNCTION__)) != S_OK) {
            __leave;
        }

        //{
            //#define BUF_SZ (16)
            //ULONG sz = BUF_SZ;
            //CHAR buf[BUF_SZ];
            //if ((Status = DebugControl->Input(buf, sz, &sz)) == S_OK) {
            //    DebugControl->Output(DEBUG_OUTPUT_NORMAL, "dude, awesome camera stuff: %s\n", buf);
            //}
        //}
        if (args) {
            if ((args[0] == '-' || args[0] == '/') && args[1] == 'a') {
                IsAll = TRUE;
            }
        } else { // args is never NULL
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "!cam1 usage: wusup\n");
            return Status;
        }

        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "yo dude: %s\n", cmd);

        Status = DebugControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, "x iacamera64!used_mem_list", DEBUG_EXECUTE_ECHO);
        Status = DebugSymbols->GetOffsetByName("iacamera64!used_mem_list", &Info);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "list: %I64x\n", Info);
        Status = DebugDataSpaces->ReadVirtual(Info, &list, sizeof(LIST_ENTRY), &BytesRead);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "flink: %I64x\n", list.Flink);
        size_t full_len = strlen(cmd) + 4*sizeof(ULONG64);
        char* full_cmd = new char[full_len];
        listAddr = (ULONG64) list.Flink;
        sprintf_s(full_cmd, full_len, "%s %I64x", cmd, listAddr);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "yoyo: %d len %d %s\n", full_len, strlen(full_cmd), full_cmd);
        Status = DebugControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, full_cmd, DEBUG_EXECUTE_ECHO);


        if ((Status = DebugDataSpaces->ReadVirtual(CamMemList, &camera, sizeof(CAM), &BytesRead)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Could not read camera memory list.\n");
            __leave;
        }
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "dude, awesome camera stuff: %d args: %s isall: %d\n", 1, args, IsAll);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "more stuff: cam1: %x, cam2: %x, cam3: %x, cam4: %x, at %I64x\n", 
                                        camera.dude1, camera.dude2, camera.dude3, camera.dude4, CamMemList);

        if ((Status = DebugSymbols->GetOffsetByName("nt!NtImageInfo", &Info)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "no Info.\n");
            __leave;
        }
        if ((Status = DebugDataSpaces->ReadVirtual(Info, &ntInfo, sizeof(NT_IMAGE_INFO), &BytesRead)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "no Info data.\n");
            __leave;
        }

        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: Version             %x\n",    ntInfo.Version);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: OsMajorVersion      %x\n",    ntInfo.OsMajorVersion);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: OsMinorVersion      %x\n",    ntInfo.OsMinorVersion);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: MajorRelease        %x\n",    ntInfo.MajorRelease);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: LoaderBlockSize     %x\n",    ntInfo.LoaderBlockSize);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: LoaderExtensionSize %x\n",    ntInfo.LoaderExtensionSize);

        /*
         *  fffff804`64a42190 nt!KiProcessListHead = struct _LIST_ENTRY [ 0xffff9b88`230ff390 - 0xffff9b88`46444410 ]
         */ 
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: dude1\n");
        if ((Status = DebugSymbols->GetOffsetByName("nt!KiProcessListHead", &ProcListAddr)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "no Proc list.\n");
            __leave;
        }
        // cast to LIST_ENTRY, and get Flink
        //
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: dude2\n");
        if ((Status = DebugDataSpaces->ReadVirtual(ProcListAddr, &list, sizeof(LIST_ENTRY), &BytesRead)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "no list for procs.\n");
            __leave;
        }
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Flink: %I64x\n", list.Flink);
        i = 4; // terminator for fake data
        while (list.Flink && i > 0) {
            // dump structure
            KProcAddr = (ULONG64) list.Flink;
            if ((Status = DebugDataSpaces->ReadVirtual(KProcAddr, &KProc, sizeof(KPROCESS), &BytesRead)) != S_OK) {
                DebugControl->Output(DEBUG_OUTPUT_NORMAL, "list loop fail1 %d\n", i);
                __leave;
            }
            // get next link
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "loop KProcAddr(%d): %I64x\n", i, KProcAddr);
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "loop KProc.ProcessListEntry(%d): %I64x\n", i, KProc.ProcessListEntry);
            listAddr = (ULONG64) KProc.ProcessListEntry.Flink;
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "loop listAddr(%d): %I64x\n", i, listAddr);
            if ((Status = DebugDataSpaces->ReadVirtual(listAddr, &list, sizeof(LIST_ENTRY), &BytesRead)) != S_OK) {
                DebugControl->Output(DEBUG_OUTPUT_NORMAL, "list loop fail2 %d\n", i);
                __leave;
            }
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "loop Flink(%d): %I64x\n", i, list.Flink);
            i--;
        }
    }

    __finally {
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "ntInfo: BAM!!!\n");
        ReleaseInterfaces();
    }
    return Status;
}

HRESULT
CALLBACK
cam2(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    HRESULT Status = S_OK;
    UNREFERENCED_PARAMETER(args);
    __try {
        if ((Status = QueryInterfaces(DebugClient)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Could not query interfaces.\n");
            __leave;
        }
        if ((Status = IsKernelMode(DebugClient, __FUNCTION__)) != S_OK) {
            __leave;
        }
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "dude, awesome camera stuff: %d\n", 2);
    }
    __finally {
        ReleaseInterfaces();
    }
    return Status;
}
