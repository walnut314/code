//+-------------------------------------------------------------------------
//
//  Copyright (C) Intel, 1023
//
//  File:       camext.h
//
//--------------------------------------------------------------------------

#include <ntddk.h>
#include <windef.h>
#include <ks.h>

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
    ApiVersion = { 5, 5, EXT_API_VERSION_NUMBER64, 0 };

static WINDBG_EXTENSION_APIS ExtensionApis;

static USHORT SavedMajorVersion;
static USHORT SavedMinorVersion;
static USHORT CameraVersion = 13489;


