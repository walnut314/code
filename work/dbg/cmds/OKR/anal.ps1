# AutoPST\ImagePostInstallPatch\c$\Tools\PST\AnalyzeLiveKernelDumps.ps1
# Emulation

if ($env:_NT_KD_PATH) {
    Set-Alias kd $env:_NT_KD_PATH
} else {
    Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
}

if ($args.count -eq 0) {
    $DumpFile = "F:\Intel_Dev\Dumps\BC_15F\ADLP-PSTS3_DripsWatchdog-20220502-1339\ADLP-PSTS3_DripsWatchdog-20220502-1339.dmp"
    Write-Host "you didn't provide a dump, so I'm using: $($DumpFile)"
} else {
    $DumpFile = $args[0];
}

$LogFileName = "analysis.log"
kd -z $DumpFile -logo $LogFileName -lines -c "!sym prompts;.reload;.load jsprovider.dll;.scriptload C:\sandbox\dump\eval.js;dx Debugger.State.Scripts.eval.Contents.EvalDump();.scriptunload eval.js;q"

