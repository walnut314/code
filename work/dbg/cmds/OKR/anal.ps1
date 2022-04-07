
Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
#$DumpFile = "dump\IND220201EV1DK_DripsWatchdog-20220309-0400.dmp"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_9F\MEMORY_DMP\dump\MEMORY.DMP"
$DumpFile = "D:\Intel_Dev\Dumps\BC_133\MEMORY_CHE221105EV2DK_2022-04-06_00-58-44.DMP"
$DbgCmds = "`$`$< eval.kd"
$LogFile = "analysis.log"

# https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/kd-command-line-options
#kd -c $DbgCmds -noshell -loga $LogFile -z $DumpFile

kd -c $DbgCmds -noshell -z $DumpFile


