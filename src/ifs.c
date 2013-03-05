#include "ifs.h"

/**
Iterated Function System
*/
int draw_ifs(void* data)
{
    SDL_mutex* lock = (SDL_mutex*)data;
    fractal_t* f = fthread.f;
    SDL_Surface* s = fthread.s;
    int i, j;

    // DEBUG
    /*/
    printf("size %u %u\n", f->width, f->height);
    printf("scale %lf\n", f->scale);
    printf("num_iter %u\n", f->iter);
    printf("position %lf %lf\n", f->position.x, f->position.y);
    //*/
    /*/
    printf("num_tr %u\n", f->param.ifs.num_tr);
    for(i = 0; i < f->param.ifs.num_tr; i++)
    {
        printf("tr %d coefficients ", i);
        for(j = 0; j < 7; j++)
            printf("%.2lf ", f->param.ifs.tr[i].tr[j]);
        printf("\n");
    }
    //*/
    // END DEBUG */

    double x, y, oldx;
    double rand_n;
    ifs_tr_t* tr;
    SDL_Surface* px;
    SDL_Rect px_pos;

    px = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0, 0, 0, 0);

    clock_t update_clock = clock() + CLOCKS_PER_SEC/20;

    tr = f->param.ifs.tr;

    x = y = 0;

    double ratio_hw = (double)f->height/(double)f->width;
    double ratio_wh = (double)f->width/(double)f->height;
    double ratio_h2 = f->height/2.0;
    double ratio_w2 = f->width/2.0;
    double deltax = -f->position.x;
    double deltay =  f->position.y;
    double scale = f->scale/((f->width >= f->height) ? ratio_w2 : ratio_h2);
    deltax = -ratio_w2*(1.0 + f->position.x);
    deltay = -ratio_h2*(1.0 + f->position.y);

    for(i = 0; i < f->iter; i++)
    {
        rand_n = rand()/(double)RAND_MAX;

        for (j = 0; j < f->param.ifs.num_tr; j++)
        {
            if (tr[j].proba >= rand_n) break;
            rand_n -= tr[j].proba;
        }

        oldx = x;
        x = tr[j].xx*x    + tr[j].xy*y + tr[j].x*s->w;
        y = tr[j].yx*oldx + tr[j].yy*y + tr[j].y*s->h;

        px_pos.x = (int)((ratio_hw*(scale*(x+deltax)) + 1)*ratio_w2);
        px_pos.y = (int)((-ratio_wh*(scale*(y+deltay)) + 1)*ratio_h2);

        if(px_pos.x >= 0 && px_pos.y >= 0 && px_pos.x < f->width && px_pos.y < f->height)
        {
            SDL_FillRect(px, NULL, 0x88EE33);
            SDL_BlitSurface(px, NULL, s, &px_pos);
        }


        /* surveillance FPS */
        if(i%50000 == 0 && clock() > update_clock)
        {
            if(!fthread.interrupter)
            {
                SDL_mutexP(lock);
                fthread.drawing = 0;
                SDL_mutexV(lock);
                break;
            }
            SDL_Flip(s);
            while(update_clock <= clock())
                update_clock += CLOCKS_PER_SEC/24;
        }
    }
    SDL_Flip(s);
    SDL_FreeSurface(px);
    return 0;
}
