#include "orbit.h"

/**
Met à jour l'aperçu de la fractale de Julia correspondant à l'emplacement de la souris sur l'ensemble de Mandelbrot

@param f ensemble de Mandelbrot
@param screen l'écran
*/
void update_julia_preview(fractal_t* mbrot, SDL_Surface* screen)
{
    if(fpthread.running && fpthread.drawing) return;
    fthread_wait();
    fpthread_wait();
    fpthread_draw(mbrot, screen);
}

/**
Initialise et effectue le lancement du dessin de l'aperçu (Julia)

@param f fractale de Julia
@param s surface où déssiner
*/
void draw_escape_time_no_thread(fractal_t* f, SDL_Surface* s)
{
    fthread_wait();
    fpthread_wait();
    fthread.f = f;
    fthread.s = s;
    draw_escape_time(fthread.lock);
}

Uint16 julia_preview_mouse_x = 0, julia_preview_mouse_y = 0;

/**
Met à jour la constante de Julia en fonction de la position de la souris

@param f ensemble de Mandelbrot
@param screen l'écran
*/
void update_julia_constant(fractal_t* mbrot, SDL_Surface* screen)
{
    R2vec c = surface_px2complex(mbrot, screen, mouse_x, mouse_y);
    SDL_mutexP(fthread.lock);
    mbrot->param.mbrot.julia->param.julia.c = c;
    SDL_mutexV(fthread.lock);
    julia_preview_mouse_x = mouse_x;
    julia_preview_mouse_y = mouse_y;
}

/**
Converti les coordonées d'un pixel d'une surface en nombre complexe
@param f ensemble de Julia ou Mandelbrot
@param s surface de dessin
@param x coordonnée du pixel de gauche à droite
@param y coordonnée du pixel de haut en bas
@return position correspondante (de la fractale)
*/
R2vec surface_px2complex(fractal_t* f, SDL_Surface* s, Uint16 x, Uint16 y)
{
    // coordonnées initiales, coordonnées actuelles, coordonnées finales, pas par pixel vertical et horizontal
    double x0, y0; // coordonnées initiales
    double x1, y1; // coordonnées finales
    double hstep, vstep; // pas par pixel (vertical et horizontal)
    double xscale, yscale; // échelle sur chaque axe (l'échelle de l'axe x est basée sur l'axe y)
    R2vec point;

    yscale = f->scale; // l'échelle de référence est sur l'axe vertical
    xscale = yscale*s->h/s->w;

    // étendue du graphe à la bonne échelle : (x0, y0) à (x1, y1)
    x0 = -1./xscale+f->position.x;
    y0 = -1./yscale+f->position.y;
    x1 = 1./xscale+f->position.x;
    y1 = 1./yscale+f->position.y;

    hstep = (x1-x0)/s->w;
    vstep = (y1-y0)/s->h;

    point.x = x0 + hstep*x;
    point.y = y0 + vstep*y;

    return point;
}

/**
Permet de mettre à jour l'aperçu de Julia (appelée lorsqu'on déplace la souris)
Gérée par un thread
*/
int jpt_func(void* data)
{
    fractal_t* mbrot = fpthread.f;
    SDL_Surface* screen = fpthread.s;
    Uint16 preview_w = screen->w / 3.5;
    Uint16 preview_h = screen->h / 3.5;
    SDL_Surface* screen_corner = SDL_CreateRGBSurface(SDL_HWSURFACE, preview_w, preview_h, 32, 0, 0, 0, 0);
    SDL_Rect corner_position;
    corner_position.x = screen->w - preview_w;
    corner_position.y = screen->h - preview_h;
    update_julia_constant(current_fractal(), screen);
    SDL_mutexP(fpthread.lock);
    draw_escape_time_no_thread(mbrot->param.mbrot.julia, screen_corner);
    SDL_BlitSurface(screen_corner, NULL, screen, &corner_position);
    SDL_Flip(screen);
    SDL_mutexV(fpthread.lock);
    SDL_FreeSurface(screen_corner);
    fpthread.drawing = 0;
    return 0;
}

/**
Ici on va dessiner une fractale de Mandelbrot ou de Julia sur une surface.
Gérée par un thread
*/
int draw_escape_time(void* data)
{
    //SDL_mutex* lock = (SDL_mutex*)data;
    fractal_t* f = fthread.f;
    SDL_Surface* s = fthread.s;

    double x, y; // coordonnées courantes dans les boucles de déplacement par pixel
    double x0, y0;
    double x1, y1;
    double hstep, vstep;
    double xscale, yscale;

    // colone et ligne courante
    unsigned int col, row;

    SDL_Surface* px; // pixel à coller
    SDL_Rect px_pos; // position où coller le pixel
    clock_t update_clock = clock() + CLOCKS_PER_SEC/24.0; // on met à jour l'image toutes les 0.2s

    yscale = f->scale;
    xscale = yscale*(double)s->h/(double)s->w;

    x0 = -1.0/xscale+f->position.x;
    y0 = -1.0/yscale+f->position.y;
    x1 = 1.0/xscale+f->position.x;
    y1 = 1.0/yscale+f->position.y;

    hstep = (x1-x0)/(double)s->w;
    vstep = (y1-y0)/(double)s->h;

    px = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0, 0, 0, 0);

    x = x0; // on se place aux coordonnées initiales
    y = y0;

    /*
    L'itération commence ici. On parcours chaque pixel de l'écran.
    Si une interruption du thread est demandée, on s'arrête à la fin d'une ligne
    */
    for(row = 0;
        (row <= s->h)
        && (fpthread.interrupter || !fpthread.running)
        && (fthread.interrupter || !fthread.running);
        row++)
    {
        px_pos.y = row; // on met à jour la position du pixel à coller
        y = y0 + vstep*row; // on utilise le pas vertical pour se déplacer dans le plan lorsqu'on passe à la ligne suivante
        for (col = 0; col <= s->w; col++)
        {
            px_pos.x = col;
            x = x0 + hstep*col;

            SDL_FillRect(px, NULL, orbit_c[iterate_orbit_fractal(f, x, y)*767/f->iter]); // on applique la couleur
            SDL_BlitSurface(px, NULL, s, &px_pos); // on colle le pixel au bon endroit
        }
        /* surveillance FPS */
        if(clock() > update_clock)
        {
            SDL_Flip(s);
            while(update_clock <= clock())
                update_clock += CLOCKS_PER_SEC/24.0;
        }
    }
    SDL_Flip(s); // on met à jour l'image lorsqu'elle est complète
    SDL_FreeSurface(px);

    fthread.drawing = 0;
    return 0;
}

/**
La présence d'un point dans la fractale doit se ressentir dans son graphe
@param f ensemble de Mandelbrot ou de Julia
@param x0 position sur l'axe réel du point
@param y0 position sur l'axe imaginaire du point
@return entier entre 0 et 767 déterminant la couleur du point
*/
unsigned int iterate_orbit_fractal(fractal_t* f, const double x0, const double y0)
{
    double x, y, previous_x, cx, cy;
    unsigned int p; // nombre d'itérations avant échappement

    x = x0;
    y = y0;

    cx = f->type == ft_mandelbrot ? x0 : f->param.julia.c.x;
    cy = f->type == ft_mandelbrot ? y0 : f->param.julia.c.y;

    for (p = 0; p < f->iter; p++)
    {
        previous_x = x;
        x = x*x - y*y + cx;
        y = 2*previous_x*y + cy;
        if(x*x + y*y > 4) // Le module de l'affixe du point doit être supérieur à 2 (rayon d'échappement)
            return p;
    }
    return 0;
}

/**
Stocke en mémoire les couleurs calculée pour les ensembles de Mandelbrot ou Julia
*/
void init_orbit_colors()
{
    Uint8 r, g, b; // rouge, vert et bleu
    Uint16 i, p;

    for(i = 0; i <= 767; i++)
    {
        p = i;
        g = p < 256 ? p : 255;
        p -= g;
        r = p < 256 ? p : 255;
        b = p - r;
        orbit_c[i] = r*65536 + g*256 + b;
    }
}

/**
On défini ici les niveaux de rouge, vert et bleu à un certain point
On veux que la somme des trois niveaux soit proportionnelle au facteur de présence du point dans la fractale
On veux donc satisfaire une équation de la forme R+V+B = P
On défini un ordre de priorité pour les couleurs : vert, rouge, bleu
@return couleur du point
*/
Uint32 orbit_fractal_position_color(fractal_t* f, SDL_Surface* s, const double x, const double y)
{
    return orbit_c[iterate_orbit_fractal(f, x, y)*767/f->iter];
}
