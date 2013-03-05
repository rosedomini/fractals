#ifndef ORBIT_H_INCLUDED
#define ORBIT_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "fconfig.h"
#include "main.h"

extern Uint16 julia_preview_mouse_x, julia_preview_mouse_y;
void update_julia_constant(fractal_t* mbrot, SDL_Surface* screen);

R2vec surface_px2complex(fractal_t* f, SDL_Surface* s, Uint16 x, Uint16 y);

void update_julia_preview(fractal_t* mbrot, SDL_Surface* screen);

void draw_escape_time_no_thread(fractal_t* f, SDL_Surface* s);

int jpt_func(void* data);
int draw_escape_time(void* data);

unsigned int iterate_orbit_fractal(fractal_t* f, const double x0, const double y0);

Uint32 orbit_c[768];
void init_orbit_colors();

Uint32 orbit_fractal_position_color(fractal_t* f, SDL_Surface* s, const double x, const double y);

#endif // ORBIT_H_INCLUDED
