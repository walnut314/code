rem "g:\Program Files\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe" sign /fdws /di a b

rem "g:\Program Files\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe" sign /f cert /fd sha256 /dg . MyFile.dll
"g:\Program Files\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe" sign /fd sha256 /dg . MyFile.dll
