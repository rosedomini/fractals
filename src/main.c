/*
Projet sur les fractales

Dominic ROSE
Minh Huy NGUYEN
*/
#include "main.h"

unsigned int mouse_x = 0;
unsigned int mouse_y = 0;

/**
Programme principal

@param argc nombre de paramètres
@param argv paramètres
Seul le premier est utilisé : l'emplacement absolu ou relatif d'un fichier de configuration compatible
*/
int main(int argc, char** argv)
{
    // on initialise SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }

    srand(time(NULL));

    load_fractal_config(argc, argv); // on charge les paramètres

    SDL_Surface* screen = initVideo(current_fractal()->width, current_fractal()->height, 0); // on initialise la vidéo
    if(!screen)
        return -1;

    atexit(SDL_Quit); // pour quitter proprement

    init_orbit_colors(); // précalcule 768 couleurs pour colorier les ensembles de Mandelbrot ou Julia

    display_fractal(current_fractal(), screen); // on affiche la fractale

    // boucle d'attente d'évènements
    int done = 0;
    int fullscreen = 0;
    int next = 0;
    int refresh = 0;
    int change_fractal;
    fractal_t* f;
    unsigned int old_w, old_h;
    R2vec mpos;
    double mx, my;
    SDL_mutex* lock = SDL_CreateMutex();
    fthread.lock = lock;
    fpthread.lock = lock;
   /* SDL_Event last_event;
    clock_t cclock = clock();
    clock_t event_clock = cclock - CLOCKS_PER_SEC;*/

    while(!done)
    {
        change_fractal = 0;
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
        	/*if(event.type == SDL_KEYDOWN)
        	{
		        cclock = clock();
		        if(cclock < event_clock 
		        	&& event.key.keysym.sym == last_event.key.keysym.sym) 
		        		continue;
		        last_event = event;
		        event_clock = cclock + CLOCKS_PER_SEC;
            }*/
            switch(event.type)
            {
                case SDL_QUIT:
                    done = 1;
                break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE: // quitter
                            done = 1;
                        case SDLK_SPACE:
                        case SDLK_RETURN:
                        case SDLK_F5: // actualiser/regénérer
                        	refresh = 1;
                        break;
                        case SDLK_RIGHT:
                        case SDLK_UP:
                        case SDLK_PAGEUP:
                            next = 1;
                        case SDLK_LEFT:
                        case SDLK_DOWN:
                        case SDLK_PAGEDOWN:
                            change_fractal = 1;
                        break;
                        case SDLK_F11:
                            event.type = SDL_VIDEORESIZE;
                            if(fullscreen)
                            {
                                event.resize.w = old_w;
                                event.resize.h = old_h;
                                fullscreen = 0;
                            }
                            else
                            {
                                old_w = screen->w;
                                old_h = screen->h;
                                event.resize.w = 1600; // detectable ?
                                event.resize.h = 900;
                                fullscreen = 1;
                            }
                        break;
                        default:
                        break;
                    }
                break;

                case SDL_MOUSEMOTION:
                    if(current_fractal()->type == ft_mandelbrot)
                    {
                        SDL_mutexP(lock);
                        mouse_x = event.motion.x;
                        mouse_y = event.motion.y;
                        SDL_mutexV(lock);
                        update_julia_preview(current_fractal(), screen);
                    }
                break;

                case SDL_MOUSEBUTTONUP:
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                    switch(event.button.button)
                    {
                        case 1: // clique gauche
                        case 3: // clique droit
                            f = current_fractal();
                            switch(f->type)
                            {
                                case ft_mandelbrot:
                                    fthread_stop();
                                    fpthread_stop();
                                    fthread_wait();
                                    fpthread_wait();
                                    SDL_mutexP(lock);
                                    set_current_fractal(f->param.mbrot.julia);
                                    SDL_mutexV(lock);
                                    display_fractal(current_fractal(), screen);
                                break;
                                case ft_julia:
                                    SDL_mutexP(lock);
                                    set_current_fractal(f->param.julia.mbrot);
                                    SDL_mutexV(lock);
                                    display_fractal(current_fractal(), screen);
                                    update_julia_preview(current_fractal(), screen);
                                break;
                                default:
                                    if(event.button.button == 1) next = 1;
                                    change_fractal = 1;
                                break;
                            }
                        break;
                        case 2: // clique du milieu
                        	refresh = 1;
                        break;
                        case 4: // roulette de la souris vers le haut : zoom+
                        case 5: // roulette de la souris vers le bas : zoom-
                            fthread_stop();
                            fpthread_stop();
                            fthread_wait();
                            fpthread_wait();
                            f = current_fractal();
                            if(f->type == ft_mandelbrot || f->type == ft_julia)
                            {
                                mpos = surface_px2complex(f, screen, mouse_x, mouse_y);
                                mx = mpos.x;
                                my = mpos.y;
                            }
                            else
                            {
                                mx = (2.0*(double)mouse_x/(double)screen->w - 1.0)/f->scale + f->position.x;
                                my = -(2.0*(double)mouse_y/(double)screen->h - 1.0)/f->scale + f->position.y;
                            }
                            if(event.button.button == 4)
                            {
                                f->position.x = (3.0*f->position.x + mx)/4.0;
                                f->position.y = (3.0*f->position.y + my)/4.0;
                                f->scale *= 1.33333;
                            }
                            else
                            {
                                f->position.x = (4.0*f->position.x - mx)/3.0;
                                f->position.y = (4.0*f->position.y - my)/3.0;
                                f->scale /= 1.33333;
                            }
                            SDL_FillRect(screen, NULL, 0);
                            display_fractal(f, screen);
                        default:
                        break;
                    }
                break;
                default:
                break;
            }
            
            if(refresh)
            {
            	refresh = 0;
                fthread_stop();
                fpthread_stop();
                fthread_wait();
                fpthread_wait();
                SDL_FillRect(screen, NULL, 0);
                f = current_fractal();
                if(f->type == ft_flame) flame_reinit(f);
                display_fractal(f, screen);
            }

            if(change_fractal)
            {
                fthread_stop();
                fpthread_stop();
                fthread_wait();
                fpthread_wait();
                if(next) next_fractal();
                else previous_fractal();
                SDL_FreeSurface(screen);
                f = current_fractal();
                screen = initVideo(f->width, f->height, 0);
                if(!screen) return -1;
                SDL_FillRect(screen, NULL, 0);
                display_fractal(f, screen);
                change_fractal = 0;
                next = 0;
            }

            // on met à jour la vidéo lors d'un redimensionnement de la fenêtre
            if(event.type == SDL_VIDEORESIZE)
            {
                fthread_stop();
                fpthread_stop();
                fthread_wait();
                fpthread_wait();
                f = current_fractal();
                /*printf("fractal w %u h %u\n", f->width, f->height);
                printf("screen w %u h %u\n", screen->w, screen->h);
                printf("new screen w %u h %u\n", event.resize.w, event.resize.h);*/
                f->width = (unsigned int)((double)f->width*(double)event.resize.w/(double)screen->w);
                f->height = (unsigned int)((double)f->height*(double)event.resize.h/(double)screen->h);
                //printf("new fractal w %u h %u\n", f->width, f->height);
                SDL_FreeSurface(screen); // on libère la précédente vidéo
                screen = initVideo(event.resize.w, event.resize.h, fullscreen); // on réinitialise la vidéo
                if(!screen)
                    return -1;
                display_fractal(f, screen);
            }
        }
        usleep(5000);
        if(current_fractal()->type == ft_mandelbrot && (julia_preview_mouse_x != mouse_x || julia_preview_mouse_y != mouse_y))
            update_julia_preview(current_fractal(), screen);
    }

    fthread_stop();
    fpthread_stop();
    fthread_wait();
    fthread_free();
    fpthread_wait();
    fpthread_free();
    SDL_DestroyMutex(lock);

    return 0;
}


/**
Initialise la vidéo

@param w largeur en pixels de la fenêtre à afficher
@param h hauteur en pixels de la fenêtre à afficher
@param fullscreen bool déterminant si on affiche la vidéo en plein écran ou non (fonction désactivée par manque de compatibilité avec certaines systèmes ou certaines résolutions d'écran)
*/
SDL_Surface* initVideo(int w, int h, int fullscreen)
{
    Uint32 flags = SDL_PREALLOC | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF;
    //Uint32 flags = SDL_RESIZABLE | SDL_DOUBLEBUF;
    //if(fullscreen) flags |= SDL_FULLSCREEN; // change la résolution de bureau (avec Ubuntu Unity 1600*900 en tout cas)
    SDL_Surface* screen = SDL_SetVideoMode(w, h, 32, flags);
    if (!screen)
    {
        printf("Unable to set %ix%ix32 video: %s\n", w, h, SDL_GetError());
        return NULL;
    }
    return screen;
}

/**
On affiche une fractale sur la fenêtre avec un titre correspondant

@param f objet de la fractale à déssiner
@param screen surface où déssiner (l'écran)
*/
void display_fractal(fractal_t* f, SDL_Surface* screen)
{
    /*/
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);//*/

    switch(f->type)
    {
        case ft_mandelbrot:
            SDL_WM_SetCaption("Fractales - Ensemble de Mandelbrot (à temps d'échappement)", NULL);
        break;
        case ft_julia:
            SDL_WM_SetCaption("Fractales - Ensemble de Julia (à temps d'échappement)", NULL);
        break;
        case ft_ifs:
            SDL_WM_SetCaption("Fractales - IFS (Iterated Function System)", NULL);
        break;
        case ft_flame:
            SDL_WM_SetCaption("Fractales - Algorithmes de flammes", NULL);
        break;
        default:
            printf("Type de fractale inconnu.");
            exit(1);
        break;
    }

    //fthread_stop();
    //fpthread_stop();
    fthread_draw(f, screen);

    /*/ thread => faux temps
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("Executed in %.9fs\n", timespecdiff(&t1, &t0)/1000000000);//*/
}
