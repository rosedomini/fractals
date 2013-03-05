
#include "fthread.h"

fthread_t fthread = {NULL, NULL, NULL, NULL, 0, 0, 0}; // thread pour dessiner les fractales
fthread_t fpthread = {NULL, NULL, NULL, NULL, 0, 0, 0}; // un deuxième pour l'aperçu (preview) (Julia)

int(*fthread_fun_list[4])(void*) = {
    draw_escape_time,
    draw_escape_time,
    draw_ifs,
    draw_flame
};

/**
Initialise le thread principal avec la fonction permettant de dessiner sa fractale sur sa surface
*/
void fthread_draw(fractal_t* f, SDL_Surface* s)
{
    fthread_wait();
    fpthread_wait();
    //SDL_mutexP(fthread.lock);
    fthread.f = f;
    fthread.s = s;
    fthread.interrupter = 1;
    fthread.running = 1;
    fthread.drawing = 1;
    //SDL_mutexV(fthread.lock);
    fthread.thread = SDL_CreateThread(fthread_fun_list[f->type], fthread.lock);
}

/**
Demande l'interruption du thread (lors d'un zoom par exemple)
*/
void fthread_stop()
{
    //SDL_mutexP(fthread.lock);
    fthread.interrupter = 0;
    //SDL_mutexV(fthread.lock);
}

/**
Souvent appelée après fthread_stop(), permet de rejoindre (attendre) que le thread quitte sa fonction et se ferme.
*/
void fthread_wait()
{
    if(fthread.running)
    {
        SDL_WaitThread(fthread.thread, NULL);
        fthread.thread = NULL;
    }
    SDL_mutexP(fpthread.lock);
    fthread.interrupter = 0;
    fthread.running = 0;
    fthread.drawing = 0;
    SDL_mutexV(fpthread.lock);
}

void fthread_free()
{
    fthread_wait();
    //free_fractal(fthread.f);
    //SDL_FreeSurface(fthread.s);
    fthread.thread = NULL;
    fthread.f = NULL;
    fthread.s = NULL;
}

void fpthread_draw(fractal_t* f, SDL_Surface* s)
{
    fpthread_wait();
    fthread_wait();
    SDL_mutexP(fpthread.lock);
    fpthread.f = f;
    fpthread.s = s;
    fpthread.interrupter = 1;
    fpthread.running = 1;
    fpthread.drawing = 1;
    SDL_mutexV(fpthread.lock);
    fpthread.thread = SDL_CreateThread(jpt_func, fpthread.lock);
}

void fpthread_stop()
{
    SDL_mutexP(fpthread.lock);
    fpthread.interrupter = 0;
    SDL_mutexV(fthread.lock);
}

void fpthread_wait()
{
    if(fthread.running)
    {
        SDL_WaitThread(fpthread.thread, NULL);
        fpthread.thread = NULL;
    }
    SDL_mutexP(fpthread.lock);
    fpthread.interrupter = 0;
    fpthread.running = 0;
    fpthread.drawing = 0;
    SDL_mutexV(fpthread.lock);
}

void fpthread_free()
{
    fpthread_wait();
    free(fpthread.thread);
    //free_fractal(fpthread.f);
    //SDL_FreeSurface(fpthread.s);
    fpthread.thread = NULL;
    fpthread.f = NULL;
    fpthread.s = NULL;
}
