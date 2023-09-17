msbuild.exe pst.sln /M /T:clean /P:Platform="x64";Configuration="debug" /flp:Verbosity=Diagnostic
msbuild.exe pst.sln /M /T:clean /P:Platform="x64";Configuration="release" /flp:Verbosity=Diagnostic
