.assembly extern mscorlib
{
  .ver 4:0:0:0
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 ) // .z\V.4..
}
.assembly extern System.Windows.Forms
{
    .publickeytoken = ()B7 7A 5C 56 19 34 E0 89)
    .ver  4:0:0:0
}
.assembly 'Cil'
{
  .custom instance void class [mscorlib]System.Runtime.CompilerServices.RuntimeCompatibilityAttribute::'.ctor'() =  (
		01 00 01 00 54 02 16 57 72 61 70 4E 6F 6E 45 78   // ....T..WrapNonEx
		63 65 70 74 69 6F 6E 54 68 72 6F 77 73 01       ) // ceptionThrows.

  .hash algorithm 0x00008004
  .ver  0:0:0:0
}
.module Cil.exe // GUID = {3E71F859-3A56-42FD-A4B1-B35EEA5FC1F0}


  .class private auto ansi beforefieldinit Program
  	extends [mscorlib]System.Object
  {

    // method line 1
    .method public hidebysig specialname rtspecialname 
           instance default void '.ctor' ()  cil managed 
    {
        // Method begins at RVA 0x2050
	// Code size 7 (0x7)
	.maxstack 8
	ldarg.0 
	call instance void object::'.ctor'()
	ret 
    } // end of method Program::.ctor

    // method line 2
    .method private static hidebysig 
           default void Main ()  cil managed 
    {
        // Method begins at RVA 0x2058
	.entrypoint
	// Code size 17 (0x11)
	.maxstack 8
    ldstr "Yo sup homi!"
    call void class [mscorlib]System.Console::WriteLine(string)
    nop
	ldstr "Hello CIL code!"
	call void class [mscorlib]System.Console::WriteLine(string)
	call string class [mscorlib]System.Console::ReadLine()
	pop 
	ret 
    } // end of method Program::Main

  } // end of class Program

