# https://www.ervik.as/automating-dump-file-analysis-via-powershell-for-citrix-xenapp/

Set-Alias kd "\Program Files (x86)\Windows Kits\10\Debuggers\x64\kd.exe"
$DumpFile = "C:\dev\Dumps\Athos_UcmUcsiCx\repro_171\ATHK0212601\ATHK0212601_UcmUcsiCx.sys-20211011-1923.dmp"
#$debugcommands = “!analyze -v; q”
$debugcommands = "`$`$< thrd.kd"
$logfile = "dump.log”
kd -c $debugcommands -noshell -logo $logfile -z $DumpFile

