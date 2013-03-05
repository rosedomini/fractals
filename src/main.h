#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL/SDL.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "orbit.h"
#include "ifs.h"
#include "fconfig.h"
#include "test.h"
#include "fthread.h"
#include "fconfig.h"

extern unsigned int mouse_x;
extern unsigned int mouse_y;

int main(int argc, char** argv);

SDL_Surface* initVideo(int w, int h, int fullscreen);

void display_fractal(fractal_t* fractal, SDL_Surface* screen);

#endif // MAIN_H_INCLUDED
