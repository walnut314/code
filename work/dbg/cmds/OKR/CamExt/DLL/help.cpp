/*++

--*/

#include "stdafx.h"
#include "common.h"

HRESULT
CALLBACK
help(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    PDEBUG_CONTROL4 DebugControl;

    UNREFERENCED_PARAMETER(args);

    if (DebugClient->QueryInterface(__uuidof(IDebugControl), (PVOID *)&DebugControl) == S_OK) {

        HMODULE hModule = GetModuleHandle(APPNAME);

        if (hModule) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "\nCopyright © 2023 Intel Corp. All rights reserved.\n\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "help       - Displays this list.\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "mem        - Displays camera memory in use\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "frames     - Displays queued camera frames\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "test       - Development command\n");
            //DebugControl->Output(DEBUG_OUTPUT_NORMAL, "mem_used_p - Displays camera memory in use (private) \n");
        }

        DebugControl->Release();
    }

    return S_OK;
}
