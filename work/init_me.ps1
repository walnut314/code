#https://seankilleen.com/2020/04/how-to-create-a-powershell-alias-with-parameters/
#function setdir($path) {set-location $path}
#foreach ($profileLocation in ($PROFILE | Get-Member -MemberType NoteProperty).Name)
#{
#    Write-Host "$($profileLocation): $($PROFILE.$profileLocation)"
#}

$DriveLetter = "F:"
if ($args.count -eq 0) {
    Write-Host "you didn't provide a drive letter, so I'm using: $($DumpFile)"
} else {
    $DriveLetter = $args[0];
}
Write-Host "using drive letter: " $DriveLetter

$a = Get-Module PSReadLine
if ($a) {
    Remove-Module PSReadLine
}

function global:Run-Lily {
    set-location -Path "c:\sandbox"
    #Clear-Host
    Get-Location
    Write-Host "lilypad"
    ./lilypad.py
}

doskey /exename=powershell.exe lily=Run-Lily
doskey /exename=powershell.exe dev=Set-Location -Path "$env:SystemDrive\dev"
doskey /exename=powershell.exe sand=Set-Location -Path "c:\sandbox"
doskey /exename=powershell.exe sym=Set-Location -Path "c:\symbols"
doskey /exename=powershell.exe tools=Set-Location -Path "c:\sandbox\tools"
doskey /exename=powershell.exe wdk=Set-Location -Path "c:\sandbox\wdk"
doskey /exename=powershell.exe grok=Set-Location -Path "C:\dev\code\toys\grokkage"
doskey /exename=powershell.exe inc=Set-Location -Path "C:\Program Files (x86)\Windows Kits\10\Include"
doskey /exename=powershell.exe work=Set-Location -Path "C:\dev\code\work"
doskey /exename=powershell.exe dump=Set-Location -Path $DriveLetter"\Intel_Dev\Dumps"
doskey /exename=powershell.exe largo=Set-Location -Path $DriveLetter"\Intel_Dev\Bugs\Largo"
doskey /exename=powershell.exe ffu=Set-Location -Path $DriveLetter"\Intel_Dev\Bugs\Largo\FFU_Image"
doskey /exename=powershell.exe charts=Set-Location -Path $DriveLetter"\Intel_Dev\Bugs\Charts"
doskey /exename=powershell.exe bug=Set-Location -Path $DriveLetter"\Intel_Dev\Bugs"
doskey /exename=powershell.exe pst=Set-Location -Path "C:\dev\AutoPST"
doskey /exename=powershell.exe dbg=Set-Location -Path "C:\dev\code\work\dbg\docs"
doskey /exename=powershell.exe wordl=Set-Location -Path "C:\dev\code\toys\misc\wordl"
doskey /exename=powershell.exe okr=Set-Location -Path "C:\dev\Code\work\dbg\cmds\OKR"
doskey /exename=powershell.exe prj=Set-Location -Path "C:\dev\code\toys\cbox\algo\PRJ"
doskey /exename=powershell.exe home=Set-Location -Path "$env:USERPROFILE"
doskey /exename=powershell.exe macs=doskey /macros:all
doskey /exename=powershell.exe pur=del *~

