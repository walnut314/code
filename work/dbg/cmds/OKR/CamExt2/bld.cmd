rem msbuild.exe /bl /t:sdv pst.sln /M /P:Platform="x64";Configuration="debug" /flp:Verbosity=Diagnostic /p:RunCodeAnalysis=true

msbuild.exe /bl PstKdExt.sln /M /P:Platform="x64";Configuration="debug" /flp:Verbosity=Diagnostic /p:RunCodeAnalysis=false
