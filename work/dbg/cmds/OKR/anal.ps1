# https://www.ervik.as/automating-dump-file-analysis-via-powershell-for-citrix-xenapp/

Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
#$DumpFile = "dump\IND220201EV1DK_DripsWatchdog-20220309-0400.dmp"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_9F\MEMORY_DMP\dump\MEMORY.DMP"
$DumpFile = "D:\Intel_Dev\Dumps\BC_133\MEMORY_CHE221105EV2DK_2022-04-06_00-58-44.DMP"
$DbgCmds = "`$`$< eval.kd"
$LogFile = "dump.log"
kd -c $DbgCmds -noshell -logo $LogFile -z $DumpFile

<#
#Get-ChildItem -Path C:\dev\Dumps -Filter *.dmp -Recurse
#>

