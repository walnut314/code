#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// https://docs.microsoft.com/en-us/windows/win32/sysinfo/access-uefi-firmware-variables-from-a-universal-windows-app
// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-enumsystemfirmwaretables
// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemfirmwaretable
// https://www.codeguru.com/cpp/misc/misc/system/article.php/c12347/SMBIOS-Demystified.htm
// https://gist.github.com/Meitinger/65d321765590f85c70bccdf12bccab16

#include <stdio.h>
#include <windows.h>
//#include <winbase.h>
//#include <tchar.h>
#include <stdio.h>
//#include <strsafe.h>
#include <string.h>

#define UEFI_VARIABLE_RUNTIME_MODE      (L"CurrentMsSurfaceUefiRuntimeModeVariableName")
#define UEFI_MFG_GUID                   (L"{3811BE0C-6AEF-44DD-9382-CD99A24C6619}")

void hexdump(const unsigned char *buffer, unsigned long long length)
{
    char OffsetBuf[10];
    unsigned long long LastStart = 0, i, k, j;
    unsigned long long elf_bias = -62;
    for (i = 0 ; i < length ; i++) {
        if (i % 16 == 0) {
            sprintf(OffsetBuf, "%08llx", i);
            printf("%4s:", OffsetBuf);
        }
        if (i % 4 == 0) {
            printf(" ");
        }
        printf("%02x ", buffer[i]);
        if (i % 16 == 15 || i == length-1) {
            if (i == length-1) {
                for (k = i % 16 ; k < 15 ; k++) {
                     printf( "   " );
                     if (k % 4 == 3 && k % 16 != 0) {
                         printf(" ");
                     }
                }
            }
            printf("|");
            for (j = LastStart ; j <= i ; j++) {
                if (j % 4 == 0) {
                    printf(" ");
                }
                if (buffer[j] > 31 && buffer[j] < 127) {
                    printf("%c", buffer[j]);
                } else {
                    printf(".");
                }
            }
            if (i == length-1) {
                printf("\n");
                break;
            }
            printf("\n");
            LastStart += 16;
        }        
    }
}


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

void do_uefi2()
{
    static const wchar_t *g_kszSmbiosAssetTagVariableName = L"Type3.AssetTag";
    static const wchar_t g_kszSurfaceFirmwareCommonGuid[] = L"{d2e0b9c9-9860-42cf-b360-f906d5e0077a}";
    wchar_t g_szAssetTagAscii[256];
    DWORD dwAttributes;

    // Get existing asset tag in NVRAM.
    // This also functions as a test to see if the version of UEFI has been updated to support Asset Tags.
    // At boot time, the UEFI boot will always set the Asset Tag variable in NVRAM, even if just a blank string.
    // ERROR_NOACCESS means that the UEFI has not been updated to support Asset Tags yet.
    // The asset tag must always be an ASCII string with a null terminator explicitly stored at the end.
    SetLastError(0);
    DWORD dwNvramSize =
        GetFirmwareEnvironmentVariableExW(
            g_kszSmbiosAssetTagVariableName,
            g_kszSurfaceFirmwareCommonGuid,
            &g_szAssetTagAscii,
            sizeof(g_szAssetTagAscii),
            &dwAttributes);

    DWORD dwLastError = GetLastError();
    if (dwLastError == ERROR_ENVVAR_NOT_FOUND) {
        printf("failed %d uefi upgrade required\n", dwLastError);
        //fwprintf_s(stderr, L"%s\r\n", SafeLoadString(IDS_STRING_UEFI_UPGRADE_REQUIRED));
        //DWORD nReturnCode = AssetTagReturnCode::UefiUpgradeRequired;
        goto exit;
    } else d        printf("got it: %x size: %d\n", dwAttributes, g_szAssetTagAscii);
        hexdump(g_szAssetTagAscii, dwNvramSize);
    }
exit:
    return;
}

#define BUFSIZE 256
#define FAC_TABLE_TAG   ((DWORD) 'PCAF')
#define ACPI_SIG        ((DWORD) 'ACPI')
#define DSDT_SIG        ((DWORD) 'DSDT')
#define ACPI 'ACPI'
 
int main(int argc, char *argv[])
{
#if 0
    const DWORD Signature = ACPI_SIG;
    UINT ret;
    char pBuffer[BUFSIZE];

    memset(pBuffer, 0, BUFSIZE);
    printf("%s\n", pBuffer);

    printf("sig %x\n", Signature);
    ret = EnumSystemFirmwareTables('ACPI', (PVOID) pBuffer, BUFSIZE);
    //ret = EnumSystemFirmwareTables('RSMB', (PVOID) pBuffer, BUFSIZE);
    printf("enum %s ret %d\n", pBuffer, ret);
    hexdump(pBuffer, ret);
    //memset(pBuffer, 0, BUFSIZE);
    //ret = GetSystemFirmwareTable(Signature, DSDT_SIG, (PVOID) pBuffer, BUFSIZE);
    //printf("tbl %s ret %d\n", pBuffer, ret);
#endif
    //do_uefi();
    do_uefi2();
    return 0;
}

