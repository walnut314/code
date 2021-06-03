#include <bits/stdc++.h>
#include <windows.h>
#include "SDL\include\sdl.h"

using namespace std;
int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
    double x = 3.14;
    double y = SDL_cos(x);
    printf("hello douche: %lf -> %lf\n", x, y);
    return 0;
}

