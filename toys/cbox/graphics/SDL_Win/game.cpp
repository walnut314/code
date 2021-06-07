#include <bits/stdc++.h>
#include <windows.h>
#include "SDL\include\sdl.h"

using namespace std;

#if 0
int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
    double x = 3.14;
    double y = SDL_cos(x);
    printf("hello douche: %lf -> %lf\n", x, y);
    return 0;
}
#endif

int APIENTRY WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("GAME",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1000, 1000, 0);
 

// triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
 
    // creates a renderer to render our images
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
 
    // creates a surface to load an image into the main memory
    SDL_Surface* surface;
 
    // please provide a path for your image
    //surface = IMG_Load("path");
 
    // loads image to our graphics hardware memory.
    //SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
 
    // clears main-memory
    //SDL_FreeSurface(surface);

// destroy texture
    //SDL_DestroyTexture(tex);
 
    while (1) ;

    // destroy renderer
    SDL_DestroyRenderer(rend);
 
    // destroy window
    SDL_DestroyWindow(win);
     
    // close SDL
    SDL_Quit();


    return 0;
}

