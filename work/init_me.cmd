@echo off

setlocal enabledelayedexpansion
rem title=authorized hacker

rem Create vector with names of days

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
title !quotes[%num%]!

set /a even=%num% %%2
rem if /i "%even%"=="1" (
rem     color 02
rem ) else (
rem     color 06
rem )
color 02

rem symbol paths
set DUDE=dude
set _NT_SYMBOL_PATH="srv*;C:\git\SurfaceOemPanelDriver\Debug\x64\bin;C:\git\SurfaceTconDriver\Debug\x64\bin"
set _NT_SOURCE_PATH="C:\git\SurfaceTconDriver\sys;C:\git\SurfaceLibrary\Kernel\DMF"

doskey ise=powershell_ise
doskey dev=cd /d %SystemDrive%\git
doskey rtos=cd /d %SystemDrive%\sandbox\rtos\FreeRTOSv10.3.1\FreeRTOSv10.3.1\FreeRTOS\Demo\WIN32-MSVC
doskey jaz=cd /d %SystemDrive%\git\Jaz-SAM
doskey zap=cd /d %SystemDrive%\git\mcu
doskey skip=cd /d %SystemDrive%\git\mcu\src\projects\zap\skip-b
doskey sis=cd /d %SystemDrive%\git\SurfaceService
doskey uefi=cd /d %SystemDrive%\git\uefi-delos
doskey avila=cd /d %SystemDrive%\git\avila-sam
doskey paloma=cd /d %SystemDrive%\git\paloma-sam
doskey haven=cd /d %SystemDrive%\git\havenu-sam
doskey cambria=cd /d %SystemDrive%\git\cambria-sam
doskey cruz=cd /d %SystemDrive%\git\cruz-sam
doskey fox=cd /d %SystemDrive%\git\foxburg-sam
doskey jupiter=cd /d %SystemDrive%\git\mcu-ple
doskey gas=cd /d %SystemDrive%\git\GasGauge\SP4_GasGaugeTelemetryDecoder
doskey sand=cd /d %SystemDrive%\sandbox
doskey tools=cd /d %SystemDrive%\sandbox\tools
doskey os=cd /d %SystemDrive%\sandbox\reactos
doskey dmf=cd /d %SystemDrive%\git\SurfaceLibrary\Kernel\DMF
doskey tcon=cd /d %SystemDrive%\git\SurfaceTconDriver
doskey oem=cd /d %SystemDrive%\git\SurfaceOemPanelDriver
doskey smf=cd /d %SystemDrive%\git\SurfaceSystemManagementFrameworkDriver
doskey sid=cd /d %SystemDrive%\git\SurfaceIntegrationDriver
doskey slib=cd /d %SystemDrive%\git\SurfaceLibrary
doskey ser=cd /d %SystemDrive%\git\SurfaceSerialHubDriver
doskey grok=cd /d C:\sandbox\code\toys\grokkage
doskey webapp=cd /d C:\sandbox\code\toys\misc\webapp
doskey inc=cd /d "C:\Program Files (x86)\Windows Kits\10\Include"
doskey fin=cd /d C:\sandbox\fin
doskey work=cd /d C:\sandbox\code\work
doskey bug=cd /d C:\sandbox\dumps
doskey tfs=cd /d C:\TFS\PLE\Users\v-babart
doskey dbg=cd /d C:\sandbox\code\work\dbg\docs
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
doskey alias=doskey /macros

@echo on

