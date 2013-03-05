#ifndef FLAME_H_INCLUDED
#define FLAME_H_INCLUDED

#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "fconfig.h"
#include "fthread.h"

int flame_alg_by_name(char* buf);
void flame_reinit(fractal_t* f);
void flame_init(fractal_t* f);

int draw_flame(void* data);

void flame_linear      (double* x, double* y); // Linear       (Variation 0)
void flame_sinusoidal  (double* x, double* y); // Sinusoidal   (Variation 1)
void flame_spherical   (double* x, double* y); // Spherical    (Variation 2)
void flame_swirl       (double* x, double* y); // Swirl        (Variation 3)
void flame_horseshoe   (double* x, double* y); // Horseshoe    (Variation 4)
void flame_polar       (double* x, double* y); // Polar        (Variation 5)
void flame_handkerchief(double* x, double* y); // Handkerchief (Variation 6)
void flame_heart       (double* x, double* y); // Heart        (Variation 7)
void flame_disc        (double* x, double* y); // Disc         (Variation 8)
void flame_spiral      (double* x, double* y); // Spiral       (Variation 9)
void flame_hyperbolic  (double* x, double* y); // Hyperbolic   (Variation 10)
void flame_diamond     (double* x, double* y); // Diamond      (Variation 11)
void flame_ex          (double* x, double* y); // Ex           (Variation 12)
void flame_julia       (double* x, double* y); // Julia        (Variation 13)
void flame_bent        (double* x, double* y); // Bent         (Variation 14)
void flame_waves       (double* x, double* y); // Waves        (Variation 15)
void flame_fisheye     (double* x, double* y); // Fisheye      (Variation 16)
void flame_popcorn     (double* x, double* y); // Popcorn      (Variation 17)
void flame_exponential (double* x, double* y); // Exponential  (Variation 18)
void flame_power       (double* x, double* y); // Power        (Variation 19)
void flame_cosine      (double* x, double* y); // Cosine       (Variation 20)
void flame_rings       (double* x, double* y); // Rings        (Variation 21)
void flame_fan         (double* x, double* y); // Fan          (Variation 22)
void flame_blob        (double* x, double* y); // Blob         (Variation 23)
void flame_pdj         (double* x, double* y); // PDJ          (Variation 24)
void flame_fan2        (double* x, double* y); // Fan2         (Variation 25)
void flame_rings2      (double* x, double* y); // Rings2       (Variation 26)
void flame_eyefish     (double* x, double* y); // Eyefish      (Variation 27)
void flame_bubble      (double* x, double* y); // Bubble       (Variation 28)
void flame_cylinder    (double* x, double* y); // Cylinder     (Variation 29)
void flame_perspective (double* x, double* y); // Perspective  (Variation 30)
void flame_noise       (double* x, double* y); // Noise        (Variation 31)
void flame_julian      (double* x, double* y); // JuliaN       (Variation 32)
void flame_juliascope  (double* x, double* y); // JuliaScope   (Variation 33)
void flame_blur        (double* x, double* y); // Blur         (Variation 34)
void flame_gaussian    (double* x, double* y); // Gaussian     (Variation 35)
void flame_radialblur  (double* x, double* y); // RadialBlur   (Variation 36)
void flame_pie         (double* x, double* y); // Pie          (Variation 37)
void flame_ngon        (double* x, double* y); // Ngon         (Variation 38)
void flame_curl        (double* x, double* y); // Curl         (Variation 39)
void flame_rectangles  (double* x, double* y); // Rectangles   (Variation 40)
void flame_arch        (double* x, double* y); // Arch         (Variation 41)
void flame_tangent     (double* x, double* y); // Tangent      (Variation 42)
void flame_square      (double* x, double* y); // Square       (Variation 43)
void flame_rays        (double* x, double* y); // Rays         (Variation 44)
void flame_blade       (double* x, double* y); // Blade        (Variation 45)
void flame_secant      (double* x, double* y); // Secant       (Variation 46)
void flame_twintrian   (double* x, double* y); // Twintrian    (Variation 47)
void flame_cross       (double* x, double* y); // Cross        (Variation 48)

void flame_random      (double* x, double* y); // Random       (Random variation)

#endif // FLAME_H_INCLUDED
