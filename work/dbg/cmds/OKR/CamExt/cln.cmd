msbuild.exe PstKdExt.sln /M /T:clean /P:Platform="x64";Configuration="debug" /flp:Verbosity=Diagnostic
msbuild.exe PstKdExt.sln /M /T:clean /P:Platform="x64";Configuration="release" /flp:Verbosity=Diagnostic
