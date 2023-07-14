/*++
--*/

#include "stdafx.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extn.hpp"

static PDEBUG_CONTROL4 DebugControl;
static PDEBUG_SYMBOLS3 DebugSymbols;
static PDEBUG_DATA_SPACES4 DebugDataSpaces;

static
HRESULT
Exec(
    _In_ PCHAR cmd
    )
{
    HRESULT Status = S_OK;
    Status = DebugControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS, cmd, DEBUG_EXECUTE_ECHO);
    return Status;
}

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
test(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    HRESULT Status = S_OK;
    Extension ext(DebugClient);
    DEBUG_VALUE val;

    if (args) {
        if ((args[0] == '-' || args[0] == '/') && args[1] == 'a') {
        }
    }
    if ((Status = ext.Eval("sizeof(iacamera64!FrameContext)", &val)) != S_OK) {
        goto _Exit;
    }

_Exit:
    return Status;
}

HRESULT
CALLBACK
mem(
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
    ULONG64 total_mem = 0;
    int n_entries = 0;

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

        Status = DebugControl->SetExpressionSyntax(DEBUG_EXPR_MASM);

        Status = DebugSymbols->GetOffsetByName("iacamera64!used_mem_list", &Info);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "list: %I64x\n", Info);
        Status = DebugDataSpaces->ReadVirtual(Info, &list, sizeof(LIST_ENTRY), &BytesRead);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "flink: %I64x\n", list.Flink);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "blink: %I64x\n", list.Blink);

        Flink = (ULONG64) list.Flink;
        Blink = (ULONG64) list.Blink;

        size_t full_len = 256;
        PCHAR cmd = new char[full_len];
        
        while (Flink != Blink) {
            Status = DebugDataSpaces->ReadVirtual(Flink, &mem, sizeof(memory_range_t), &BytesRead);
            //DebugControl->Output(DEBUG_OUTPUT_NORMAL, "host_addr   : 0x%I64x\n",  mem.host_addr);
            //DebugControl->Output(DEBUG_OUTPUT_NORMAL, "page_addr   : 0x%I64x\n",  mem.page_addr);
            //DebugControl->Output(DEBUG_OUTPUT_NORMAL, "actual_size : 0x%lx\n\n", mem.actual_size);
            sprintf_s(cmd, full_len, "dt iacamera64!_memory_range %I64x", Flink);
            Status = Exec(cmd);
            Flink = (ULONG64) mem.list.Flink;
            total_mem += mem.actual_size;
            n_entries++;
        }
        // grab the trailing link
        Status = DebugDataSpaces->ReadVirtual(Flink, &mem, sizeof(memory_range_t), &BytesRead);
        sprintf_s(cmd, full_len, "dt iacamera64!_memory_range %I64x", Flink);
        Status = Exec(cmd);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "\n", mem.actual_size);
        total_mem += mem.actual_size;
        n_entries++;

        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "total mem alloc'd         : %ld Mb\n", total_mem/1024/1024);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "number of entries in list : %d \n\n", n_entries);

    }

    __finally {
        ReleaseInterfaces();
    }
    return Status;
}

HRESULT
CALLBACK
frames(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    HRESULT Status = S_OK;
    BOOL IsAll = FALSE;
    ULONG BytesRead;
    ULONG64 Info;
    ULONG64 Count;
    ULONG32 n_frames = 0;
    unsigned int i = 0;
    DEBUG_VALUE val;
    ULONG64 size;

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

        Status = DebugSymbols->GetOffsetByName("iacamera64!g_frameContexts", &Info);
        Status = DebugSymbols->GetOffsetByName("iacamera64!g_MaxFramesToBeQueued", &Count);

        Status = DebugDataSpaces->ReadVirtual(Count, &n_frames, sizeof(ULONG32), &BytesRead);

        size_t full_len = 256;
        char* cmd = new char[full_len];
        sprintf_s(cmd, full_len, "dx -r2 (*((iacamera64!FrameContext (*)[%ld])0x%I64x))", n_frames, Info);
        Status = Exec(cmd);

        // now loop
        val.I64 = 0;
        // need to set C++ syntax first
        Status = DebugControl->SetExpressionSyntax(DEBUG_EXPR_CPLUSPLUS);
        Status = DebugControl->Evaluate("sizeof(iacamera64!FrameContext)", DEBUG_VALUE_INT64, &val, NULL);
        size = (ULONG64) val.I64;
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "\nFrame size: %I64d\n", size);

        // still need to use the directly struct here
        for (i = 0; i < n_frames; i++) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "%d>\n", i);
            sprintf_s(cmd, full_len, "dx -r2 (*((iacamera64!FrameContext *)0x%I64x))", Info);
            Status = Exec(cmd);
            Info += size;
        }
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "\nnum frames queued: %d\n", n_frames);
        
        // return to MASM syntax
        Status = DebugControl->SetExpressionSyntax(DEBUG_EXPR_MASM);
    }

    __finally {
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

        Status = DebugSymbols->GetOffsetByName("iacamera64!used_mem_list", &Info);
        Status = DebugDataSpaces->ReadVirtual(Info, &list, sizeof(LIST_ENTRY), &BytesRead);
        size_t full_len = strlen(cmd) + 4*sizeof(ULONG64);
        PCHAR full_cmd = new char[full_len];
        listAddr = (ULONG64) list.Flink;
        sprintf_s(full_cmd, full_len, "%s %I64x", cmd, listAddr);
        Exec(cmd);

    }

    __finally {
        ReleaseInterfaces();
    }
    return Status;
}


