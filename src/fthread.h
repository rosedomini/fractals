#ifndef FTHREAD_H_INCLUDED
#define FTHREAD_H_INCLUDED

#include <SDL/SDL_thread.h>
#include "fconfig.h"
#include "flame.h"
#include "ifs.h"
#include "orbit.h"

typedef struct fthread_t // thread pour dessiner les fractales
{
    SDL_Thread* thread; // SDL thread
    SDL_mutex* lock;
    fractal_t* f; // la fractale à dessiner
    SDL_Surface* s; // la surface où coller le dessin
    int running; // lancé et non rejoin
    int drawing; // lancé et actif
    int interrupter; // commande l'activité du thread
} fthread_t;

extern fthread_t fthread;
extern fthread_t fpthread;

void fthread_draw(fractal_t* f, SDL_Surface* s); // attends la fermeture du thread précédent avant de dessiner
void fthread_stop(); // demande d'arrêter le dessin
void fthread_wait(); // attends la fermeture du thread
void fthread_free();

void fpthread_draw(fractal_t* f, SDL_Surface* s); // attends la fermeture du thread précédent avant de dessiner
void fpthread_stop(); // demande d'arrêter le dessin
void fpthread_wait(); // attends la fermeture du thread
void fpthread_free();

#endif // FTHREAD_H_INCLUDED
