#include <stdio.h>
#include <windows.h>

void hello(HWND hWnd)
{
  SetWindowTextA(hWnd, ("Goodbye, World!"));
}

int main()
{
    HWND h = GetConsoleWindow();
    hello(h);
    while (1) ;
}
