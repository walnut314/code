https://stackoverflow.com/questions/20607835/setting-up-a-postmortem-debugger-for-windows-services

https://www.shafiev.com/wp/2011/08/17/post-mortem-debugging-of-windows-services

Make sure that Auto for the AeDebug key is set to 1. Also change the windbg command line to: "Path\WinDbg.exe" -p %ld –c ".dump /ma /u D:\CrashDump.dmp;qd" -e %ld –G


