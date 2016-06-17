TARGETNAME=test
TARGETTYPE=PROGRAM

UMTYPE=console
UMBASE=0x1000000
USE_LIBCMT=1

TARGETPATH=obj

INCLUDES=$(DDK_INC_PATH); \
         $(BASEDIR)\inc; \
         $(BASEDIR)\inc\ddk;

TARGETLIBS= $(DDK_LIB_PATH)\setupapi.lib \
            $(DDK_LIB_PATH)\user32.lib \
            $(DDK_LIB_PATH)\gdi32.lib \
            $(DDK_LIB_PATH)\mpr.lib

 
SOURCES=grid.c
