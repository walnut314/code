# https://www.ervik.as/automating-dump-file-analysis-via-powershell-for-citrix-xenapp/

Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
$DumpFile = "IND220201EV1DK_DripsWatchdog-20220309-0400.dmp"
$DbgCmds = "`$`$< eval.kd"
$LogFile = "dump.log"
kd -c $DbgCmds -noshell -logo $LogFile -z $DumpFile

<#
#Get-ChildItem -Path C:\dev\Dumps -Filter *.dmp -Recurse
#>

