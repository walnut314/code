#cls
Remove-Variable * -ea 0
$ErrorActionPreference = 'stop'
#requires -runasadmin

$drvId = 0
$marshal = [Runtime.InteropServices.Marshal]
$getSmartVersion = '0x074080'

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

$handle = $kernel32::CreateFile("\\.\PhysicalDrive$DrvId", [uint32]'0xc0000000', 3, [System.IntPtr]::Zero, 3, 64, [System.IntPtr]::Zero);
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
$ptrInBuffer  = [System.IntPtr]::Zero
$inBufferSize = 0

# outBuffer:
$smartVersionStruct = New-Object GETVERSIONINPARAMS_EX
$outBufferSize = $marshal::SizeOf($smartVersionStruct)
$ptrOutBuffer  = $marshal::AllocHGlobal($OutBufferSize)

<#
$resultSize = 0
$ioControlCode = [uint32]$getSmartVersion
if ($kernel32::DeviceIoControl($handle, $ioControlCode, $ptrInBuffer, $inBufferSize, $ptrOutBuffer, $outBufferSize, [ref]$resultSize, [System.IntPtr]::Zero)) {
    $smartVersionStruct = $marshal::PtrToStructure($ptrOutBuffer, [type]'GETVERSIONINPARAMS_EX')
    $smartVersionStruct | ft -AutoSize
}
#>
$null = $kernel32::CloseHandle($handle)

# now the same for an USB-connected SSD:
$mediaList = gwmi -namespace root\Microsoft\Windows\Storage -class MSFT_PhysicalDisk
$usbMedia  = $mediaList | ?{$_.BusType -eq 7}

$diskList = gwmi -namespace root\cimv2 –class Win32_DiskDrive
$usbDisk  = $diskList.where({$_.Index -eq $usbMedia.DeviceId})

$usbMapping = gwmi -query "SELECT Antecedent,Dependent FROM Win32_USBControllerDevice"
$mapping    = @($usbMapping).where({([wmi]$_.Dependent).PnPDeviceId -eq $usbDisk.PnPDeviceId})
$usbHost    = [wmi]$mapping.Antecedent

# what should come next?
# getting the 'Root-Hub-Name' or not?
# check if UAS/USAP (USB Attached SCSI Protocol) is supported?
# IoControlCode = SCSCI-PassThrough or SCSIPassThroughDirect (each with or without Buffer)?
