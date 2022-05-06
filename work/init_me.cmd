@echo off

setlocal enabledelayedexpansion
rem title=intel

rem Create vector with names of days

if %1.==. goto Exit
set DriveLetter=%1
echo %DriveLetter%

set i=0
for %%d in (Sunday Monday Tuesday Wednesday Thrusday Friday Saturday) do (
   set /A i=i+1
   set day[!i!]=%%d
   REM echo %%d
)


set quotes[0]=authorized hacker
set quotes[1]=arrest this man
set quotes[2]=vulnerable
set quotes[3]=victim
set quotes[4]=dialup
set quotes[5]=compromise
set quotes[6]=aethernet
set quotes[7]=insecure

set /a num=%random% %%7
echo %num%
REM echo !quotes[%num%]!
rem title !quotes[%num%]!

set /a even=%num% %%2
rem if /i "%even%"=="1" (
rem     color 02
rem ) else (
rem     color 06
rem )
color 02

rem symbol paths
set DUDE=dude
rem set _NT_SYMBOL_PATH="srv*;C:\git\SurfaceOemPanelDriver\Debug\x64\bin;C:\git\SurfaceTconDriver\Debug\x64\bin"
rem set _NT_SOURCE_PATH="C:\git\SurfaceTconDriver\sys;C:\git\SurfaceLibrary\Kernel\DMF"

doskey ise=powershell_ise
doskey dev=cd /d %SystemDrive%\dev
doskey sand=cd /d c:\sandbox
doskey lily=cd /d c:\sandbox ^&^& cd ^&^& lilypad
doskey sym=cd /d c:\symbols
doskey tools=cd /d c:\sandbox\tools
doskey wdk=cd /d c:\sandbox\wdk
doskey moltools=cd /d "C:\Users\bbartel\OneDrive - Intel Corporation\Documents\WindowsPowerShell\Modules\MOLTools"
doskey dmf=cd /d %SystemDrive%\git\SurfaceLibrary\Kernel\DMF
doskey grok=cd /d C:\dev\code\toys\grokkage
doskey inc=cd /d "C:\Program Files (x86)\Windows Kits\10\Include"
doskey work=cd /d C:\dev\code\work
doskey dump=cd /d %DriveLetter%\Intel_Dev\Dumps
doskey largo=cd /d %DriveLetter%\Intel_Dev\Bugs\Largo
doskey ffu=cd /d %DriveLetter%\Intel_Dev\Bugs\Largo\FFU_Image
doskey charts=cd /d %DriveLetter%\Intel_Dev\Bugs\Charts
doskey bug=cd /d %DriveLetter%\Intel_Dev\Bugs
doskey pst=cd /d C:\dev\AutoPST
doskey dbg=cd /d C:\dev\code\work\dbg\docs
doskey wordl=cd /d C:\dev\code\toys\misc\wordl
doskey okr=cd /d C:\dev\Code\work\dbg\cmds\OKR
doskey prj=cd /d C:\dev\code\toys\cbox\algo\PRJ
doskey home=cd /d %USERPROFILE%
doskey green=color 02
doskey yellow=color 06
doskey pur=del *~
doskey cat=type $*
doskey ll=dir $*
doskey l=dir $*
doskey ls=dir $*
doskey cp=xcopy $*
doskey mv=move $*
doskey rm=del $*
doskey grep=findstr $*
doskey ddk=C:\WinDDK\7600.16385.1\bin\setenv C:\WinDDK\7600.16385.1 chk
doskey alias=doskey /macros

@echo on

:Exit

