REM 
REM cl /c /Z1 main.c
REM link /entry:EfiMain /dll /IGNORE:4086 main.obj
REM rem C:\git\UEFI-Delos\SM_MU_BASECORE\BaseTools\Bin\Mu-Basetools_extdep\Windows-x86\GenFw.exe -e PEI_CORE main.dll -o main.efi
REM C:\git\UEFI-Delos\SM_MU_BASECORE\BaseTools\Bin\Mu-Basetools_extdep\Windows-x86\GenFw.exe -e PEIM main.dll -o main.efi


rem cl.exe" /Foc:\w\53\s\Build\PalomaPkg\RELEASE_VS2017\IA32\MsSamSupportPkg\SamCapabilitiesPei\SamCapabilitiesPei\OUTPUT\.\ /nologo /arch:IA32 /c /WX /GS- /W4 /Gs32768 /D UNICODE /O1b2 /GL /FIAutoGen.h /EHs-c- /GR- /GF /Gw /MP /D RELEASE_TARGET /D EFI_DEBUG /D DEBUG_MODE=1 /DWA_00228 /DWA_1216713 /DWA_1299407 /DWA_1399699 /D SHIP_MODE=1 /D MS_CHANGE_ACTIVE=1 /D NSOCKETS=2 /D UCODE_FLAG=1   /D MODERN_STANDBY_SUPPORT /Ic:\w\53\s\Common\SM_SURF_COMMON\MsSamSupportPkg\SamCapabilitiesPei  /Ic:\w\53\s\Build\PalomaPkg\RELEASE_VS2017\IA32\MsSamSupportPkg\SamCapabilitiesPei\SamCapabilitiesPei\DEBUG  /Ic:\w\53\s\SM_MU_BASECORE\MdePkg  /Ic:\w\53\s\SM_MU_BASECORE\MdePkg\Include  /Ic:\w\53\s\SM_MU_BASECORE\MdePkg\Include\Ia32  /Ic:\w\53\s\SM_MU_BASECORE\MdeModulePkg  /Ic:\w\53\s\SM_MU_BASECORE\MdeModulePkg\Include  /Ic:\w\53\s\Common\SM_SURF_COMMON\MsSurfaceCorePkg  /Ic:\w\53\s\Common\SM_SURF_COMMON\MsSurfaceCorePkg\Include  /Ic:\w\53\s\Common\SM_SURF_COMMON\MsSamSupportPkg  /Ic:\w\53\s\Common\SM_SURF_COMMON\MsSamSupportPkg\Include  /Ic:\w\53\s\Common\SM_SURF_COMMON\MsSamSupportPkg\SurfaceSerialHub c:\w\53\s\Common\SM_SURF_COMMON\MsSamSupportPkg\SamCapabilitiesPei\SamCapabilitiesPei.c c:\w\53\s\Build\PalomaPkg\RELEASE_VS2017\IA32\MsSamSupportPkg\SamCapabilitiesPei\SamCapabilitiesPei\DEBUG\AutoGen.c

cl.exe /nologo /arch:IA32 /c /WX /GS- /W3 /Gs32768 /D UNICODE /O1b2 /GL /EHs-c- /GR- /GF /Gw /MP /D RELEASE_TARGET /D EFI_DEBUG /D DEBUG_MODE=1 /DWA_00228 /DWA_1216713 /DWA_1299407 /DWA_1399699 /D SHIP_MODE=1 /D MS_CHANGE_ACTIVE=1 /D NSOCKETS=2 /D UCODE_FLAG=1   /D MODERN_STANDBY_SUPPORT main.c

link.exe /OUT:main.dll /NOLOGO /NODEFAULTLIB /IGNORE:4001 /IGNORE:4254 /OPT:REF /OPT:ICF=10 /MAP /ALIGN:32 /SECTION:.xdata,D /SECTION:.pdata,D /MACHINE:X86 /LTCG /DLL /ENTRY:EfiMain /SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER /SAFESEH:NO /BASE:0 /DRIVER /MERGE:.rdata=.data /PDB:main.pdb main.obj

C:\git\UEFI-Delos\SM_MU_BASECORE\BaseTools\Bin\Mu-Basetools_extdep\Windows-x86\GenFw.exe -e PEI_CORE main.dll -o main.efi

