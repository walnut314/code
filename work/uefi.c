#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// https://docs.microsoft.com/en-us/windows/win32/sysinfo/access-uefi-firmware-variables-from-a-universal-windows-app
// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-enumsystemfirmwaretables
// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemfirmwaretable

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#define UEFI_VARIABLE_RUNTIME_MODE      (L"CurrentMsSurfaceUefiRuntimeModeVariableName")
#define UEFI_MFG_GUID                   (L"{3811BE0C-6AEF-44DD-9382-CD99A24C6619}")

void do_uefi()
{
    DWORD                   uefiVariableSize = 0;
    UINT32                  uefiVariableValue = 0;
    uefiVariableSize = sizeof(unsigned char);
    GetFirmwareEnvironmentVariableW(UEFI_VARIABLE_RUNTIME_MODE,
                                   UEFI_MFG_GUID,
                                   (PVOID)(&uefiVariableValue),
                                   uefiVariableSize);

    printf("uefi var: %x\n", uefiVariableValue);
}

#define BUFSIZE 256
#define FAC_TABLE_TAG (DWORD) 'PCAF'
#define ACPI_SIG (DWORD) 'ACPI'

int main(int argc, char *argv[])
{
    const DWORD Signature = ACPI_SIG;
    UINT ret;
    char pBuffer[BUFSIZE];

    memset(pBuffer, 0, BUFSIZE);
    printf("%s\n", pBuffer);

    printf("sig %x\n", Signature);
    ret = EnumSystemFirmwareTables(Signature, (PVOID) pBuffer, BUFSIZE);
    printf("enum %s ret %d\n", pBuffer, ret);
    memset(pBuffer, 0, BUFSIZE);
    ret = GetSystemFirmwareTable(Signature, 'DSDT', (PVOID) pBuffer, BUFSIZE);
    printf("tbl %s ret %d\n", pBuffer, ret);

    do_uefi();
    return 0;
}

