/*++

--*/

#include "stdafx.h"
#include "common.h"


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
cam1(
    _In_ PDEBUG_CLIENT DebugClient,
    _In_opt_ PCSTR args
    )
{
    HRESULT Status = S_OK;
    BOOL IsAll = FALSE;
    ULONG64 CamMemList = 0xffffbc0db8935c70;
    ULONG BytesRead;
    CAM camera;

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
        }

        if ((Status = DebugDataSpaces->ReadVirtual(CamMemList, &camera, sizeof(CAM), &BytesRead)) != S_OK) {
            DebugControl->Output(DEBUG_OUTPUT_NORMAL, "Could not read camera memory list.\n");
            __leave;
        }
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "dude, awesome camera stuff: %d args: %s isall: %d\n", 1, args, IsAll);
        DebugControl->Output(DEBUG_OUTPUT_NORMAL, "more stuff: cam1: %x, cam2: %x, cam3: %x, cam4: %x, at %I64x\n", 
                                        camera.dude1, camera.dude2, camera.dude3, camera.dude4, CamMemList);
    }

    __finally {
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
