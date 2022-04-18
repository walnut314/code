# https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/kd-command-line-options

if ($env:_NT_KD_PATH) {
    Set-Alias kd $env:_NT_KD_PATH
} else {
    Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
}

#write-host "There are a total of $($args.count) arguments"
#for ( $i = 0; $i -lt $args.count; $i++ ) {
#    write-host "Argument  $i is $($args[$i])"
#} 

if ($args.count -eq 0) {
#$DumpFile = "D:\Intel_Dev\Dumps\BC_9F\MEMORY_DMP\dump\MEMORY.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_101\MEMORY_ZAC214703EV1_2022-01-31_12-33-21\MEMORY_ZAC214703EV1_2022-01-31_12-33-21.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_119\MEMORY_CHE221107EV2DK_2022-04-04_04-25-11.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_133\MEMORY_CHE221105EV2DK_2022-04-06_00-58-44.DMP"
$DumpFile = "D:\Intel_Dev\Dumps\BC_15F\IND220201EV1DK_DripsWatchdog-20220309-0400\IND220201EV1DK_DripsWatchdog-20220309-0400.dmp"
    Write-Host "you didn't provide a dump, so I'm using: $($DumpFile)"
    #Exit
} else {
    $DumpFile = $args[0];
}

$DbgCmds = "`$`$< eval.kd"
$LogFile = "analysis.log"

#kd -c $DbgCmds -noshell -loga $LogFile -z $DumpFile
kd -c $DbgCmds -noshell -z $DumpFile


