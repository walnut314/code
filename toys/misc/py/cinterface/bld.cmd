rem #!/bin/bash

rem # https://cygwin.com/cygwin-ug-net/dll.html

rem #g++ -dynamiclib -flat_namespace foo.cpp -o libfoo.so

rem # for c dll
gcc -c foofoo.c
gcc -shared -o foofoo.dll foofoo.o

rem # for cpp dll
rem g++ -c foo.cpp
rem g++ -shared -o foo.dll foo.o

