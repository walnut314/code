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

            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "dude, wusup: %d\n", 77);
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "help - displays this list.\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "st   - Displays system service table.\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "idt  - Displays interrupt descriptor table\n\n");
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "cam  - Displays camera stuff\n\n");
        }

        DebugControl->Release();
    }

    return S_OK;
}
