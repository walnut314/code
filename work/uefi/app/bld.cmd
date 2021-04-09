REM 
REM cl /c /Z1 main.c
REM link /entry:EfiMain /dll /IGNORE:4086 main.obj
REM rem C:\git\UEFI-Delos\SM_MU_BASECORE\BaseTools\Bin\Mu-Basetools_extdep\Windows-x86\GenFw.exe -e PEI_CORE main.dll -o main.efi
REM C:\git\UEFI-Delos\SM_MU_BASECORE\BaseTools\Bin\Mu-Basetools_extdep\Windows-x86\GenFw.exe -e PEIM main.dll -o main.efi

REM cl.exe /nologo /arch:IA32 /c /WX /GS- /W3 /Gs32768 /D UNICODE /O1b2 /GL /EHs-c- /GR- /GF /Gw /MP /D RELEASE_TARGET /D EFI_DEBUG /D DEBUG_MODE=1 /DWA_00228 /DWA_1216713 /DWA_1299407 /DWA_1399699 /D SHIP_MODE=1 /D MS_CHANGE_ACTIVE=1 /D NSOCKETS=2 /D UCODE_FLAG=1   /D MODERN_STANDBY_SUPPORT main.c

REM try this one
"\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\bin\Hostx64\x64\cl.exe"  /nologo /c /WX /GS- /W3 /Gs32768 /D UNICODE /O1b2s /GL /Gy /EHs-c- /GR- /GF /Gw /MP /D RELEASE_TARGET /D EFI_DEBUG /D DEBUG_MODE=1 /DWA_00228 /DWA_1216713 /DWA_1299407 /DWA_1399699 /D SHIP_MODE=1 /D MS_CHANGE_ACTIVE=1 /D NSOCKETS=2 /D UCODE_FLAG=1   /D MODERN_STANDBY_SUPPORT  main.c

REM link.exe /OUT:main.dll /NOLOGO /NODEFAULTLIB /IGNORE:4001 /IGNORE:4254 /OPT:REF /OPT:ICF=10 /MAP /ALIGN:32 /SECTION:.xdata,D /SECTION:.pdata,D /MACHINE:X86 /LTCG /DLL /ENTRY:EfiMain /SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER /SAFESEH:NO /BASE:0 /DRIVER /MERGE:.rdata=.data /PDB:main.pdb main.obj

REM try this one
"\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\bin\Hostx64\x64\link.exe" /OUT:main.dll /NOLOGO /NODEFAULTLIB /IGNORE:4001 /IGNORE:4281 /IGNORE:4254 /OPT:REF /OPT:ICF=10 /MAP /ALIGN:32 /SECTION:.xdata,D /SECTION:.pdata,D /Machine:X64 /LTCG /DLL /ENTRY:EfiMain /SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER /SAFESEH:NO /BASE:0 /DRIVER /MERGE:.rdata=.data /PDB:main.pdb /PDBALTPATH:main.pdb /ALIGN:4096 main.obj

REM https://edk2-docs.gitbook.io/edk-ii-basetools-user-guides/genfw
REM 
C:\git\UEFI-Delos\SM_MU_BASECORE\BaseTools\Bin\Mu-Basetools_extdep\Windows-x86\GenFw.exe -v -e UEFI_APPLICATION main.dll -o main.efi

REM still getting error: Image is not an application
REM
REM https://wiki.osdev.org/UEFI#Binary_Format
objdump --all-headers main.efi > main.dmp
