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
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "help       - displays this list.\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "mem_used   - Also displays camera memory in use (public) \n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "mem_used_p - Displays camera memory in use (private) \n");
        }

        DebugControl->Release();
    }

    return S_OK;
}
