# https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/kd-command-line-options
# AutoPST\ImagePostInstallPatch\c$\Tools\PST\AnalyzeLiveKernelDumps.ps1

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
#$DumpFile = "C:\sandbox\dump\dogfood\IND220201EV1DK_DripsWatchdog-20220309-0400.dmp" # LKD_0x15F
#$DumpFile = "C:\sandbox\dump\dogfood\MEMORY_CHE221107EV2DK_2022-04-04_04-25-11.DMP" # 0x119
#$DumpFile = "D:\Intel_Dev\Dumps\BC_A\MEMORY_TIN194703ES277_2020-02-03_17-41-15.DMP" # 0xA
#$DumpFile = "D:\Intel_Dev\Dumps\BC_3D_2\MEMORY_ZAC214711EV1_2022-01-05_09-21-12\MEMORY_ZAC214711EV1_2022-01-05_09-21-12.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_9F\MEMORY_DMP\dump\MEMORY.DMP"
#$DumpFile = "C:\sandbox\dump\dogfood\MEMORY_IND221202EV2DK_2022-04-22_20-58-48.DMP"
#$DumpFile = "C:\sandbox\dump\dogfood\ZAC220916EV2DK_UcmUcsiCx.sys-20220427-1234.dmp"
$DumpFile = "\\cce-rpt-data\Debug\22ww18.4_Zaca_UCM-UCSI_YB_LKR_Dumps\ZAC220916EV2DK_2022-04-26_13-10\LKRD\ZAC220916EV2DK_UcmUcsiCx.sys-20220427-1234.dmp"
#$DumpFile =  "\\cce-rpt-data\debug\21ww42.1_Athos_PAGE_FAULT_IN_NONPAGED_AREA_(50)\MEMORY_ATHK0212648_2021-10-11_04-10-14.DMP"
#$DumpFile =  "\\cce-rpt-data\debug\22ww14.5_Chehalis_VIDEO_SCHEDULER_INTERNAL_ERROR_(119)\MEMORY_CHE221107EV2DK_2022-04-04_04-25-11.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_101\MEMORY_ZAC214703EV1_2022-01-31_12-33-21\MEMORY_ZAC214703EV1_2022-01-31_12-33-21.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_119\MEMORY_CHE221107EV2DK_2022-04-04_04-25-11.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_133\MEMORY_CHE221105EV2DK_2022-04-06_00-58-44.DMP"
#$DumpFile = "D:\Intel_Dev\Dumps\BC_15F\IND220201EV1DK_DripsWatchdog-20220309-0400\IND220201EV1DK_DripsWatchdog-20220309-0400.dmp"
    Write-Host "you didn't provide a dump, so I'm using: $($DumpFile)"
    #Exit
} else {
    $DumpFile = $args[0];
}

$DbgCmds = "`$`$< eval.kd"
$LogFileName = "analysis.log"

#kd -c $DbgCmds -noshell -logo $LogFileName -z $DumpFile

kd -c $DbgCmds -noshell -z $DumpFile

# simulate AutoPST -- use the 2nd one
#kd -z $DumpFile -logo $LogFileName -lines -c "!sym prompts;.reload;!analyze -v;.ecxr;!for_each_frame dv /t;q"
#kd -z $DumpFile -logo $LogFileName -lines -c "!sym prompts;.reload;.load jsprovider.dll;.scriptload C:\sandbox\dump\eval.js;dx Debugger.State.Scripts.eval.Contents.EvalDump();.scriptunload eval.js;q"
