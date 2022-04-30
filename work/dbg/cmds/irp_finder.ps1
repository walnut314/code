if ($env:_NT_KD_PATH) {
    Set-Alias kd $env:_NT_KD_PATH
} else {
    Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
}

if ($args.count -eq 0) {
$DumpFile = "\\cce-rpt-data\Debug\22ww18.4_Zaca_UCM-UCSI_YB_LKR_Dumps\ZAC220916EV2DK_2022-04-26_13-10\LKRD\ZAC220916EV2DK_UcmUcsiCx.sys-20220427-1234.dmp"
    Write-Host "you didn't provide a dump, so I'm using: $($DumpFile)"
    #Exit
} else {
    $DumpFile = $args[0];
}

$LogFileName = "analysis.log"

kd -z $DumpFile -logo $LogFileName -lines -c "!sym prompts;.reload;!irpfind;q"
