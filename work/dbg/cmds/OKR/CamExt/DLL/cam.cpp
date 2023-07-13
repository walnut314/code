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
    ULONG64 Flink;
    ULONG64 Blink;
    memory_range_t mem;

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

        Status = DebugSymbols->GetOffsetByName("iacamera64!used_mem_list", &Info);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "list: %I64x\n", Info);
        Status = DebugDataSpaces->ReadVirtual(Info, &list, sizeof(LIST_ENTRY), &BytesRead);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "flink: %I64x\n", list.Flink);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "blink: %I64x\n", list.Blink);

        Flink = (ULONG64) list.Flink;
        Blink = (ULONG64) list.Blink;

        while (Flink != Blink) {
            Status = DebugDataSpaces->ReadVirtual(Flink, &mem, sizeof(memory_range_t), &BytesRead);
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "host_addr   : 0x%I64x\n",  mem.host_addr);
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "page_addr   : 0x%I64x\n",  mem.page_addr);
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "actual_size : 0x%llx\n\n", mem.actual_size);
            Flink = (ULONG64) mem.list.Flink;
        }
        // grab the trailing link
        Status = DebugDataSpaces->ReadVirtual(Flink, &mem, sizeof(memory_range_t), &BytesRead);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "host_addr   : 0x%I64x\n", mem.host_addr);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "page_addr   : 0x%I64x\n", mem.page_addr);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "actual_size : 0x%lx\n\n", mem.actual_size);

    }

    __finally {
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "mem_used2: BAM!!!\n");
        ReleaseInterfaces();
    }
    return Status;
}

// this is for use with privates
HRESULT
CALLBACK
mem_used_p(
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


