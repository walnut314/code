#cls
Remove-Variable * -ea 0
$ErrorActionPreference = 'stop'
#requires -runasadmin

$marshal = [Runtime.InteropServices.Marshal]
$Do_PST_IOCTL = '0x9C412408'

$kernel32 = Add-Type -Name 'kernel32' -Namespace 'Win32' -PassThru -MemberDefinition @"
[DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
public static extern IntPtr CreateFile(
    String lpFileName,
    UInt32 dwDesiredAccess,
    UInt32 dwShareMode,
    IntPtr lpSecurityAttributes,
    UInt32 dwCreationDisposition,
    UInt32 dwFlagsAndAttributes,
    IntPtr hTemplateFile);
[DllImport("Kernel32.dll", SetLastError = true)]
public static extern bool DeviceIoControl(
    IntPtr   hDevice,
    uint     oControlCode,
    IntPtr   InBuffer,
    uint     nInBufferSize,
    IntPtr   OutBuffer,
    uint     nOutBufferSize,
    ref uint pBytesReturned,
    IntPtr  Overlapped);
[DllImport("kernel32.dll", SetLastError=true)]
public static extern bool CloseHandle(IntPtr hObject);
"@

$handle = $kernel32::CreateFile("\\.\PstFilt", [uint32]'0xc0000000', 3, [System.IntPtr]::Zero, 3, 64, [System.IntPtr]::Zero);
if ([int]$handle -lt 1) {throw 'cannot get handle.'}
else { write-host "dude wusup"; }

# struct for SMART-version:
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public struct GETVERSIONINPARAMS_EX {
   public Byte   bVersion;
   public Byte   bRevision;
   public Byte   bReserved;
   public Byte   bIDEDeviceMap;
   public UInt32 fCapabilities;
   public UInt32 dwDeviceMapEx;
   public UInt16 wIdentifier;
   public UInt16 wControllerId;
   public UInt64 dwReserved;
 };
"@

# inBuffer:
$inBufferSize = 32
$ptrInBuffer  = $marshal::AllocHGlobal($inBufferSize)

# outBuffer:
$smartVersionStruct = New-Object GETVERSIONINPARAMS_EX
$outBufferSize = $marshal::SizeOf($smartVersionStruct)
$ptrOutBuffer  = $marshal::AllocHGlobal($OutBufferSize)

$resultSize = 0
$ioControlCode = [uint32]$Do_PST_IOCTL
if ($kernel32::DeviceIoControl($handle, 
                               $ioControlCode, 
                               $ptrInBuffer, 
                               $inBufferSize, 
                               $ptrOutBuffer, 
                               $outBufferSize, 
                               [ref]$resultSize, 
                               [System.IntPtr]::Zero)) {

    $smartVersionStruct = $marshal::PtrToStructure($ptrOutBuffer, [type]'GETVERSIONINPARAMS_EX')
    $smartVersionStruct | ft -AutoSize
}

$null = $kernel32::CloseHandle($handle)


