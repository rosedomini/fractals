
#include "flame.h"

double xx, xy, xc, yx, yy, yc;

void(*algorithms[FLAME_NUM_VARIATION])(double*, double*) = {
    flame_linear, // 0
	flame_sinusoidal, // 1
	flame_spherical, // 2
	flame_swirl, // 3
	flame_horseshoe, // 4
	flame_polar, // 5
	flame_handkerchief, // 6
	flame_heart, // 7
	flame_disc, // 8
	flame_spiral, // 9
	flame_hyperbolic, // 10
	flame_diamond, // 11
	flame_ex, // 12
	flame_julia, // 13
	flame_bent, // 14
	flame_waves, // 15
	flame_fisheye, // 16
	flame_popcorn, // 17
	flame_exponential, // 18
	flame_power, // 19
	flame_cosine, // 20
	flame_rings, // 21
	flame_fan, // 22
	flame_blob, // 23
	flame_pdj, // 24
	flame_fan2, // 25
	flame_rings2, // 26
	flame_eyefish, // 27
	flame_bubble, // 28
	flame_cylinder, // 29
	flame_perspective, // 30
	flame_noise, // 31
	flame_julian, // 32
	flame_juliascope, // 33
	flame_blur, // 34
	flame_gaussian, // 35
	flame_radialblur, // 36
	flame_pie, // 37
	flame_ngon, // 38
	flame_curl, // 39
	flame_rectangles, // 40
	flame_arch, // 41
	flame_tangent, // 42
	flame_square, // 43
	flame_rays, // 44
	flame_blade, // 45
	flame_secant, // 46
	flame_twintrian, // 47
	flame_cross, // 48
	flame_random // 49
};

char *alg_names[FLAME_NUM_VARIATION] = {
	"Linear",
	"Sinusoidal",
	"Spherical",
	"Swirl",
	"Horseshoe",
	"Polar",
	"Handkerchief",
	"Heart",
	"Disc",
	"Spiral",
	"Hyperbolic",
	"Diamond",
	"Ex",
	"Julia",
	"Bent",
	"Waves",
	"Fisheye",
	"Popcorn",
	"Exponential",
	"Power",
	"Cosine",
	"Rings",
	"Fan",
	"Blob",
	"PDJ",
	"Fan2",
	"Rings2",
	"Eyefish",
	"Bubble",
	"Cylinder",
	"Perspective",
	"Noise",
	"JuliaN",
	"JuliaScope",
	"Blur",
	"Gaussian",
	"RadialBlur",
	"Pie",
	"Ngon",
	"Curl",
	"Rectangles",
	"Arch",
	"Tangent",
	"Square",
	"Rays",
	"Blade",
	"Secant",
	"Twintrian",
	"Cross",
	"Random"
};

double coefficients[FLAME_NUM_VARIATION] = {
	1, // Linear
	1.5, // Sinusoidal
	6, // Spherical
	1, // Swirl
	1, // Horseshoe
	2, // Polar
	2, // Handkerchief
	2, // Heart
	4, // Disc
	4.6, // Spiral
	1.4, // Hyperbolic
	8, // Diamond
	2, // Ex
	1, // Julia (very nice: 0.5 or 1.0)
	2, // Bent
	1, // Waves
	1, // Fisheye
	1, // Popcorn
	2, // Exponential
	4, // Power
	1, // Cosine
	2, // Rings
	1.5, // Fan
	2, // Blob
	2, // PDJ
	1.5, // Fan2
	2, // Rings2
	2, // Eyefish
	2, // Bubble
	2, // Cylinder
	2, // Perspective
	2, // Noise
	1, // JuliaN
	1, // JuliaScope
	1, // Blur
	1, // Gaussian
	1, // RadialBlur
	1, // Pie
	1, // Ngon
	1, // Curl
	2, // Rectangles
	1, // Arch
	2, // Tangent
	1, // Square
	1, // Rays
	1, // Blade
	1, // Secant
	1, // Twintrian
	1, // Cross
	2 // Random (à multiplier par le coefficient de l'algorithme déterminé aléatoirement)
};

#define randinrange(min, max)(rand()%(max-min+1)+min)

int flame_alg_by_name(char* buf)
{
    int i;
    char name[100];
    for(i = 0; i < FLAME_NUM_VARIATION; i++)
    {
        strcpy(name, alg_names[i]);
        strdown(name, 0, -1);
        if(!strcmp(buf, name)) break;
    }
    if(i == FLAME_NUM_VARIATION) i = FLAME_RANDOM_ALGORITHM;
    return i;
}

/**
Génère à nouveau les coefficients de la fractale
*/
void flame_reinit(fractal_t* f)
{
    free(f->param.flame.dots);
    free(f->param.flame.alg_id_list2);
    flame_init(f);
}

/**
Initialise la fractale flamme et ses coefficients aléatoires
*/
void flame_init(fractal_t* f)
{
    int i, j;
    double proba_sum = 0;

    for(i = 0; i < 3; i++)
        f->param.flame.current_color[i] = randinrange(0, 255);

    f->param.flame.current_iter = 0;
    f->param.flame.dots = malloc(f->iter*sizeof(flame_dot_t));
    if(f->param.flame.dots == NULL)
    {
        printf("Erreur d'allocation mémoire : %lu octets\n", f->iter*sizeof(flame_dot_t));
        exit(3);
    }
    f->param.flame.current_position.x = 2.0*rand()/(double)RAND_MAX - 1.0;
    f->param.flame.current_position.y = 2.0*rand()/(double)RAND_MAX - 1.0;

    f->param.flame.num_tr = randinrange(f->param.flame.min_num_tr,
                                        f->param.flame.max_num_tr);
    f->param.flame.num_alg = randinrange(f->param.flame.min_num_alg,
                                        f->param.flame.max_num_alg);

    for(i = 0; i < f->param.flame.num_tr; i++)
    {
        if(f->param.flame.tr[i].rand_tr)
        {
            for(j = 0; j < 6; j++)
                f->param.flame.tr[i].tr[j] = rand()/(double)RAND_MAX - 0.5;
            f->param.flame.tr[i].proba = rand()/(double)RAND_MAX;
        }
        if(f->param.flame.tr[i].rand_col)
        {
            for(j = 7; j < 10; j++)
                f->param.flame.tr[i].tr[j] = rand()/(double)RAND_MAX;
        }
    }

    if(f->param.flame.proba_type == fpt_uniform)
    {
        for(i = 0; i < f->param.flame.num_tr; i++)
            f->param.flame.tr[i].proba = 1.0;
    }
    else if(f->param.flame.proba_type == fpt_random)
    {
        for(i = 0; i < f->param.flame.num_tr; i++)
            f->param.flame.tr[i].proba = rand()/(double)RAND_MAX;
    }

    for(i = 0; i < f->param.flame.num_tr; i++)
        proba_sum += f->param.flame.tr[i].proba;
    for(i = 0; i < f->param.flame.num_tr; i++)
        f->param.flame.tr[i].proba /= proba_sum;

    f->param.flame.alg_id_list2 = malloc(f->param.flame.max_num_alg*sizeof(unsigned int));
    for(i = 0; i < f->param.flame.num_alg; i++)
    {
        if(f->param.flame.alg_id_list[i] == FLAME_RANDOM_ALGORITHM)
            f->param.flame.alg_id_list2[i] = randinrange(0, FLAME_NUM_VARIATION-2);
        else
            f->param.flame.alg_id_list2[i] = f->param.flame.alg_id_list[i];
    }
}

/**
Fonction gérée par le thread principal permettant de déssiner une fractal de type flamme
*/
int draw_flame(void* data)
{
    SDL_mutex* lock = (SDL_mutex*)data;
    fractal_t* f = fthread.f;
    SDL_Surface* s = fthread.s;

    double lambda, gamma = 2.2, oldx, coef;
    int i, j, k;
    SDL_Surface* px = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 1, 32, 0, 0, 0, 0);
    SDL_Rect px_pos;

    int** freq = malloc(f->width*sizeof(int*));
    for(i = 0; i < f->width; i++)
    {
        freq[i] = malloc(f->height*sizeof(int));
        for(j = 0; j < f->height; j++)
            freq[i][j] = 0;
    }
    int max_freq = 0;

    clock_t update_clock = clock() + CLOCKS_PER_SEC/20.0;

    double ratio_hw = (double)f->height/(double)f->width;
    double ratio_wh = (double)f->width/(double)f->height;
    double ratio_h2 = f->height/2.0;
    double ratio_w2 = f->width/2.0;
    double deltax = -f->position.x;
    double deltay =  f->position.y;
    double scalex = f->scale*((f->width >= f->height) ? 1.0 : ratio_wh);
    double scaley = f->scale*((f->width >= f->height) ? ratio_hw : 1.0);

    Uint8 rgb[3];

    // DEBUG
    /*/
    printf("size %u %u\n", f->width, f->height);
    printf("scale %lf\n", f->scale);
    printf("num_iter %u\n", f->iter);
    printf("position %lf %lf\n", f->position.x, f->position.y);
    //*/
    /*/
    printf("proba_type %d\n", f->param.flame.proba_type);
    printf("current_position.x %lf current_position.y %lf\n",
           f->param.flame.current_position.x,
           f->param.flame.current_position.y);
    printf("current_color %u %u %u\n",
           f->param.flame.current_color[0],
           f->param.flame.current_color[1],
           f->param.flame.current_color[2]);

    printf("min_num_alg %u\n", f->param.flame.min_num_alg);
    printf("num_alg %u\n", f->param.flame.num_alg);
    printf("max_num_alg %u\n", f->param.flame.max_num_alg);

    for(i = 0; i < f->param.flame.num_tr; i++)
    {
        printf("tr %d rand_tr %u rand_col %u\n", i,
               f->param.flame.tr[i].rand_tr,
               f->param.flame.tr[i].rand_col);
        printf("coefficients ");
        for(j = 0; j < 10; j++)
            printf("%.2lf ", f->param.flame.tr[i].tr[j]);
        printf("\n");
    }

    printf("min_num_tr %u\n", f->param.flame.min_num_tr);
    printf("num_tr %u\n", f->param.flame.num_tr);
    printf("max_num_tr %u\n", f->param.flame.max_num_tr);
    //*/
    //
    for(i = 0; i < f->param.flame.num_alg; i++)
    {
        printf("%s (%i)", alg_names[f->param.flame.alg_id_list2[i]], f->param.flame.alg_id_list2[i]);
        if(i < f->param.flame.num_alg - 1) printf("; ");
    }
    printf("\n");
    //*/
    //*/ END DEBUG

    for(i = 20; i <= f->param.flame.current_iter; i++)
    {
        /* collage */
        px_pos.x = (int)((ratio_hw*(scalex*(f->param.flame.dots[i].position.x+deltax)) + 1)*ratio_w2);
        px_pos.y = (int)((ratio_wh*(scaley*(f->param.flame.dots[i].position.y+deltay)) + 1)*ratio_h2);

        for(k = 0; k < 3; k++)
            rgb[k] = f->param.flame.dots[i].color[k];

        if(px_pos.x >= 0 && px_pos.y >= 0 && px_pos.x < f->width && px_pos.y < f->height)
        {
            freq[px_pos.x][px_pos.y]++;
            if(freq[px_pos.x][px_pos.y] > max_freq)
                max_freq = freq[px_pos.x][px_pos.y];
            if(max_freq > 1)
                lambda = pow(log(freq[px_pos.x][px_pos.y])/log(max_freq), 1.0/gamma);
            else
                lambda = 0;
            for(k = 0; k < 3; k++) rgb[k] *= lambda;

            SDL_FillRect(px, NULL, SDL_MapRGB(s->format, rgb[0], rgb[1], rgb[2]));
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

    for(i = 0; i < 3; i++)
        rgb[i] = f->param.flame.current_color[i];

    double x = f->param.flame.current_position.x;
    double y = f->param.flame.current_position.y;

    unsigned int num_tr = f->param.flame.num_tr;
    flame_tr_t* tr = f->param.flame.tr;

    for(i = f->param.flame.current_iter+1; i < f->iter; i++)
    {
        double rand_n = rand()/(double)RAND_MAX;
        for (j = 0; j < num_tr; j++)
        {
            if(tr[j].proba >= rand_n) break;
            rand_n -= tr[j].proba;
        }

        for(k = 0; k < 3; k++)
            rgb[k] = (rgb[k] + tr[j].tr[k+7]*255)/2;

        k = f->param.flame.alg_id_list2[randinrange(0, f->param.flame.num_alg-1)];
        coef = coefficients[k];

        xx = coef*tr[j].xx;
        xy = coef*tr[j].xy;
        xc = coef*tr[j].x;
        yx = coef*tr[j].yx;
        yy = coef*tr[j].yy;
        yc = coef*tr[j].y;

        oldx = x;
        x = xx*x    + xy*y + xc;
        y = yx*oldx + yy*y + yc;

        (*algorithms[k])(&x, &y);

        if(i >= 20)
        {
            f->param.flame.dots[i].position.x = x;
            f->param.flame.dots[i].position.y = y;

            for(k = 0; k < 3; k++)
                f->param.flame.dots[i].color[k] = rgb[k];

            /* collage */
            px_pos.x = (int)((ratio_hw*(scalex*(x+deltax)) + 1)*ratio_w2);
            px_pos.y = (int)((ratio_wh*(scaley*(y+deltay)) + 1)*ratio_h2);

            if(px_pos.x >= 0 && px_pos.y >= 0 && px_pos.x < f->width && px_pos.y < f->height)
            {
                freq[px_pos.x][px_pos.y]++;
                if(freq[px_pos.x][px_pos.y] > max_freq)
                    max_freq = freq[px_pos.x][px_pos.y];
                if(max_freq > 1)
                    lambda = pow(log(freq[px_pos.x][px_pos.y])/log(max_freq), 1.0/gamma);
                else
                    lambda = 0;
                for(k = 0; k < 3; k++) rgb[k] *= lambda;

                SDL_FillRect(px, NULL, SDL_MapRGB(s->format, rgb[0], rgb[1], rgb[2]));
                SDL_BlitSurface(px, NULL, s, &px_pos);
            }
            /* surveillance FPS */
            if(i%50000 == 0 && clock() > update_clock)
            {
                if(!fthread.interrupter)
                {
                    f->param.flame.current_position.x = x;
                    f->param.flame.current_position.y = y;
                    f->param.flame.current_iter = i;
                    for(k = 0; k < 3; k++)
                        f->param.flame.current_color[k] = rgb[k];
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
    }
    SDL_Flip(s);
    SDL_FreeSurface(px);
    for(i = 0; i < f->width; i++) free(freq[i]);
    free(freq);
    return 0;
}

#define FLAME_OMEGA (rand()%2*M_PI)
#define FLAME_KSY (rand()/(double)RAND_MAX)
#define FLAME_R double r = sqrt(*x*(*x) + *y*(*y))
#define FLAME_R2 double r2 = *x*(*x) + *y*(*y);
#define FLAME_THETA double t = atan((*x)/(*y))
#define FLAME_PHY double p = atan((*y)/(*x))
#define FLAME_TR double tr = t*r
#define FLAME_OLDX double oldx = *x
#define FLAME_RSYM(x_or_y) if(rand()%2){*(x_or_y) *= -1;}
#define FLAME_SYMY(f1, f2) if(*y < 0)\
{\
    *x = (f1);\
    *y = (f2);\
}\
else\
{\
    *x = -(f1);\
    *y = -(f2);\
}
#define FLAME_SYMX(f1, f2) if(*x < 0)\
{\
    *x = (f1);\
    *y = (f2);\
}\
else\
{\
    *x = -(f1);\
    *y = -(f2);\
}

void flame_linear(double* x, double* y) // Linear (Variation 0)
{
}

void flame_sinusoidal(double* x, double* y) // Sinusoidal (Variation 1)
{
    *x = sin(*x);
    *y = sin(*y);
}

void flame_spherical(double* x, double* y) // Spherical (Variation 2)
{
    FLAME_R2;
    *x = 1.0/r2*(*x);
    *y = 1.0/r2*(*y);
}

void flame_swirl(double* x, double* y) // Swirl (Variation 3)
{
    FLAME_R2; FLAME_OLDX;
    *x = *x*sin(r2) - *y*cos(r2);
    *y = oldx*cos(r2) + *y*sin(r2);
}

void flame_horseshoe(double* x, double* y) // Horseshoe (Variation 4)
{
    FLAME_R; FLAME_OLDX;
    *x = 1.0/r*(*x - *y)*(*x + *y);
    *y = 1.0/r*2*oldx*(*y);
}

void flame_polar(double* x, double* y) // Polar (Variation 5)
{
    FLAME_THETA; FLAME_R;
    *x = 2*t/M_PI;
    *y = r - 1;
}

void flame_handkerchief(double* x, double* y) // Handkerchief (Variation 6)
{
    FLAME_THETA; FLAME_R;
    *x = r*sin(t + r);
    *y = r*cos(t - r);
}

void flame_heart(double* x, double* y) // Heart (Variation 7)
{
    FLAME_THETA; FLAME_R; FLAME_TR;
    *x = r*sin(tr);
    *y = -r*cos(tr);
}

void flame_disc(double* x, double* y) // Disc (Variation 8)
{
    FLAME_THETA; FLAME_R;
    *x = 2*t/M_PI*sin(M_PI*r);
    *y = 2*t/M_PI*cos(M_PI*r);
}

void flame_spiral(double* x, double* y) // Spiral (Variation 9)
{
    FLAME_THETA; FLAME_R;
    *x = 1.0/r*(cos(t) + sin(r));
    *y = 1.0/r*(sin(t) - cos(r));
}

void flame_hyperbolic(double* x, double* y) // Hyperbolic (Variation 10)
{
    FLAME_THETA; FLAME_R;
    /* *x = sin(t)/r;
    *y = r*cos(t);*/
    FLAME_SYMY(sin(t)/r, r*cos(t));
}

void flame_diamond(double* x, double* y) // Diamond (Variation 11)
{
    FLAME_THETA; FLAME_R;
    /* *x = sin(t)*cos(r);
    *y = cos(t)*sin(r);*/
    FLAME_SYMY(sin(t)*cos(r), cos(t)*sin(r));
}

void flame_ex(double* x, double* y) // Ex (Variation 12)
{
    FLAME_THETA; FLAME_R;
    double p03 = pow(sin(t + r), 3);
    double p13 = pow(cos(t - r), 3);
    *x = r*(p03 + p13);
    *y = r*(p03 - p13);
}

void flame_julia(double* x, double* y) // Julia (Variation 13)
{
    FLAME_THETA; FLAME_R;
    double omega = rand()%4*M_PI/2;
    *x = sqrt(r)*cos(t/2 + omega);
    *y = sqrt(r)*sin(t/2 + omega);
}

void flame_bent(double* x, double* y) // Bent (Variation 14)
{
    if(*y >= 0)
    {
        if(*x < 0)
            *x *= 2;
    }
    else
    {
        *y /= 2;
        if(x < 0)
            *x *= 2;
    }
}

void flame_waves(double* x, double* y) // Waves (Variation 15)
{
    FLAME_OLDX;
    *x = *x + xy*sin(  *y/xc/xc);
    *y = *y + yy*sin(oldx/yc/yc);
}

void flame_fisheye(double* x, double* y) // Fisheye (Variation 16)
{
    FLAME_R; FLAME_OLDX;
    const double c = 2/(r + 1);
    *x = c*(*y);
    *y = c*oldx;
}

void flame_popcorn(double* x, double* y) // Popcorn (Variation 17)
{
    FLAME_OLDX;
    *x = *x + yy*sin(tan(3*(*y)));
    *y = *y + yc*sin(tan(3*oldx));
}

void flame_exponential(double* x, double* y) // Exponential (Variation 18)
{
    const double c = exp(*x - 1.0);
    *x = c*cos(M_PI*(*y));
    *y = c*sin(M_PI*(*y));
}

void flame_power(double* x, double* y) // Power (Variation 19)
{
    FLAME_THETA; FLAME_R;
    const double c = pow(r, sin(t));
    *x = c*cos(t);
    *y = c*sin(t);
    // à ce moment : x est entre 0 et 1
    //*x = 1 - *x; less scary :o
    //*y *= -1;
    FLAME_RSYM(x); // on veut deux yeux
    /* *y -= 0.23;
    *y /= 1.8;
    *x /= 1.8;*/
}

void flame_cosine(double* x, double* y) // Cosine (Variation 20)
{
    FLAME_OLDX;
    *x =  cos(M_PI*(*x))*cosh(*y);
    *y = -sin(M_PI*oldx)*sinh(*y);
    //*x -= 0.3;
}

void flame_rings(double* x, double* y) // Rings (Variation 21)
{
    FLAME_R; FLAME_THETA;
    double c = fmod(r + xc*xc, 2*xc*xc) - xc*xc + r*(1 - xc*xc);
    //*x = c*cos(t);
    //*y = c*sin(t);
    FLAME_SYMY(c*cos(t), c*sin(t));
}

void flame_fan(double* x, double* y) // Fan (Variation 22)
{
    FLAME_THETA; FLAME_R;
    const double tc = M_PI*xc*xc;
    if( fmod(t + yc, tc) > tc/2)
    {
        //*x = r*cos(t - tc/2);
        //*y = r*sin(t - tc/2);
        FLAME_SYMY(r*cos(t - tc/2), r*sin(t - tc/2));
    }
    else
    {
        //*x = r*cos(t + tc/2);
        //*y = r*sin(t + tc/2);
        FLAME_SYMY(r*cos(t + tc/2), r*sin(t + tc/2));
    }
}

void flame_blob(double* x, double* y) // Blob (Variation 23)
{
    FLAME_THETA; FLAME_R;
    double high = 0.9, low = 0.1, waves = 5;
    double c = r*(low+(high-low)/2.0*(sin(waves*t)+1));
    *x = c*cos(t);
    *y = c*sin(t);
    FLAME_RSYM(x);
}

double pdj1 = 1.2;
double pdj2 = 2.0;
double pdj3 = 1.5;
double pdj4 = 2;

void flame_pdj(double* x, double* y) // PDJ (Variation 24)
{
    FLAME_OLDX;
    *x = sin(pdj1*(*y)) - cos(pdj2*(*x));
    *y = sin(pdj3*oldx) - cos(pdj4*(*y));
    //*x /= 2;
    //*y /= 2;
}

double fan2x = 0.8;
double fan2y = -0.33;

void flame_fan2(double* x, double* y) // Fan2 (Variation 25)
{
    FLAME_THETA; FLAME_R;
    double p1 = M_PI*fan2x*fan2x;
    double p2 = fan2y;
    if(t+p2-p1*(int)(2*t*p2/p1) > p1/2)
    {
        //*x = r*sin(t - p1/2);
        //*y = r*cos(t - p1/2);
        FLAME_SYMX(r*sin(t - p1/2), r*cos(t - p1/2));
        //FLAME_SYMY(r*sin(t - p1/2), r*cos(t - p1/2));
    }
    else
    {
        //*x = r*sin(t + p1/2);
        //*y = r*cos(t + p1/2);
        FLAME_SYMX(r*sin(t + p1/2), r*cos(t + p1/2));
        //FLAME_SYMY(r*sin(t + p1/2), r*cos(t + p1/2));
    }
    if(rand()%2)
    {
        *x *= -1;
        *y *= -1;
    }
}

const double rings2val = 0.4;

void flame_rings2(double* x, double* y) // Rings2 (Variation 26)
{
    FLAME_THETA; FLAME_R;
    double p = rings2val*rings2val;
    double t1 = r - 2*p*(double)(int)((r+p)/2/p)+r*(1-p);
    //*x = t1*sin(t);
    //*y = t1*cos(t);
    FLAME_SYMY(t1*sin(t), t1*cos(t));
    //*x /= 1.3;
    //*y /= 1.3;
}

void flame_eyefish(double* x, double* y) // Eyefish (Variation 27)
{
    FLAME_R;
    double c = 2/(r+1);
    *x *= c;
    *y *= c;
    //*x /= 1.33;
    //*y /= 1.33;
}

void flame_bubble(double* x, double* y) // Bubble (Variation 28)
{
    FLAME_R;
    double c = 4/(r*r+4);
    *x *= c;
    *y *= c;
}

void flame_cylinder(double* x, double* y) // Cylinder (Variation 29)
{
    *x = sin(*x);
    //*x /= 2.0;
}

void flame_perspective(double* x, double* y) // Perspective (Variation 30)
{
    double p1 = M_PI/180*70;
    double p2 = 10;
    double c = p2/(p2-*y*p1);
    *x *= c;
    *y *= c*cos(p1);
}

void flame_noise(double* x, double* y) // Noise (Variation 31)
{
    double k1 = FLAME_KSY;
    double k2 = FLAME_KSY;
    *x *= k1*cos(2*M_PI*k2);
    *y *= k1*sin(2*M_PI*k2);
}

void flame_julian(double* x, double* y) // JuliaN (Variation 32)
{
    FLAME_PHY; FLAME_R;
    // careful to this ratio: p1/p2
    double p1, p2; // power; distance
    p1 = 7; p2 = 7.5;
    //p1 = 800; p2 = 1000;
    //p1 = 400; p2 = 10;
    //p1 = 0.2; p2 = 0.2; // low float changes everything
    //p1 = 1.0/4; p2 = p1/1.618;
    double p3 = (int)((p1<0?-p1:p1)*FLAME_KSY);
    double t1 = (p + 2*M_PI*p3)/p1;
    double c = pow(r, p2/p1);
    *x = c*cos(t1);
    *y = c*sin(t1);
}

void flame_juliascope(double* x, double* y) // JuliaScope (Variation 33)
{
    FLAME_PHY; FLAME_R;
    double lambda = 2.0;
    double p1, p2; // power; distance
    // careful to this ratio: p1/p2
    p1 = 7; p2 = 7.5;
    //p1 = 800; p2 = 1000;
    //p1 = 400; p2 = 10;
    //p1 = 0.2; p2 = 0.2; // low float changes everything
    //p1 = 1.0/4; p2 = p1/1.618;
    double p3 = (int)((p1<0?-p1:p1)*FLAME_KSY);
    double t1 = (lambda*p + 2*M_PI*p3)/p1;
    double c = pow(r, p2/p1);
    *x = c*cos(t1);
    *y = c*sin(t1);
}

void flame_blur(double* x, double* y) // Blur (Variation 34)
{
    double k1 = FLAME_KSY;
    double k2 = FLAME_KSY;
    *x = k1*cos(2*M_PI*k2);
    *y = k1*sin(2*M_PI*k2);
}

void flame_gaussian(double* x, double* y) // Gaussian (Variation 35)
{
    /* Summing 4 random numbers and subtracting 2 is an attempt at approximating
a Gaussian distribution. */
    double c = -2;
    int i;
    for(i = 0; i < 4; i++)
        c += FLAME_KSY;
    double k5 = FLAME_KSY;
    *x = c*cos(2*M_PI*k5);
    *y = c*sin(2*M_PI*k5);
}

void flame_radialblur(double* x, double* y) // RadialBlur (Variation 36)
{
    FLAME_R; FLAME_PHY;
    double p1 = /* angle = ( */ M_PI/3 /* ) */ *M_PI/2;
    double t1 = -2;
    int i;
    for(i = 0; i < 4; i++)
        t1 += FLAME_KSY;
    // to do: t1 *= radialblur
    double t2 = p + t1*sin(p1);
    double t3 = t1*cos(p1) - 1;
    *x = r*cos(t2) + t3*(*x);
    *y = r*sin(t2) + t3*(*y);
    // to do: (x, y) /= radialblur
}

void flame_pie(double* x, double* y) // Pie (Variation 37)
{
    double p1, p2, p3; // slices; rotation; thickness
    p1 = 7; // nombre de parts
    p2 = M_PI/180*20; // 20° de rotation autours de l'axe Z
    p3 = 0.77; // ratio plein/vide
    double k1 = FLAME_KSY;
    double k2 = FLAME_KSY;
    double k3 = FLAME_KSY;
    double t1 = (int)(k1*p1+0.5);
    double t2 = p2 + 2*M_PI/p1*(t1 + k2*p3);
    *x = k3*cos(t2);
    *y = k3*sin(t2);
}

void flame_ngon(double* x, double* y) // Ngon (Variation 38)
{
    FLAME_PHY; FLAME_R;
    double p1, p2, p3, p4; // power, 2pi/sides, corners, circle
    p1 = 3;
    p2 = 1;
    p3 = 3;
    p4 = 0.2;
    p2 = 2*M_PI/p2;
    double t3 = p - p2*(int)(p/p2); // devrait être |_p/p2_|
    double t4 = t3;
    if(t3 <= p2/2)
        t4 -= p2;
    double k = (p3*(1/cos(t4) - 1) + p4)/pow(r, p1);
    *x *= k;
    *y *= k;
}

void flame_curl(double* x, double* y) // Curl (Variation 39)
{
    FLAME_OLDX;
    double p1 = 0.8;
    double p2 = 0.5;
    double t1 = 1 + p1*(*x) + p2*(*x*(*x)-*y*(*y));
    double t2 = p1*(*y) + 2*p2*(*x)*(*y);
    double c = 1/(t1*t1 + t2*t2);
    *x = c*(*x)*t1 + *y*t2;
    *y = c*(*y)*t1 - oldx*t2;
}

void flame_rectangles(double* x, double* y) // Rectangles (Variation 40)
{
    double p1, p2; // x; y
    p1 = 0.25;
    p2 = 0.5;
    // nombre de rectangles : 1/(p1*p2*4)
    *x = (2*(int)(*x/p1) + 1)*p1 - *x;
    *y = (2*(int)(*y/p2) + 1)*p2 - *y;
}

void flame_arch(double* x, double* y) // Arch (Variation 41)
{
    double k = FLAME_KSY;
    double arch = 1.0;
    *x = sin(k*M_PI*arch);
    *y = *x*(*x)/cos(k*M_PI*arch);
}

void flame_tangent(double* x, double* y) // Tangent (Variation 42)
{
    *x = sin(*x)/cos(*y);
    *y = tan(*y);
}

void flame_square(double* x, double* y) // Square (Variation 43)
{
    *x = FLAME_KSY - 0.5;
    *y = FLAME_KSY - 0.5;
}

void flame_rays(double* x, double* y) // Rays (Variation 44)
{
    FLAME_R;
    double rays = 25;
    double c = rays*tan(FLAME_KSY*M_PI*rays)/r/r;
    *x = c*cos(*x);
    *y = c*sin(*y);
}

void flame_blade(double* x, double* y) // Blade (Variation 45)
{
    FLAME_R; FLAME_OLDX;
    double k = FLAME_KSY;
    double blade = 3.0;
    double c = k*r*blade;
    *x *= cos(c) + sin(c);
    *y = oldx*(cos(c) - sin(c));
}

void flame_secant(double* x, double* y) // Secant (Variation 46)
{
    FLAME_R;
    double secant = 168; // espace interne : 1/s
    *y = 1/secant/cos(secant*r);
}

void flame_twintrian(double* x, double* y) // Twintrian (Variation 47)
{
    FLAME_R; FLAME_OLDX;
    double k = FLAME_KSY;
    double twintrian = 3;
    double c = k*r*twintrian;
    double s = sin(c);
    double t = log10(s*s) + cos(c);
    *x *= t;
    *y = oldx*(t - M_PI*s);
}

void flame_cross(double* x, double* y) // Cross (Variation 48)
{
    double c = sqrt(pow(*x*(*x) - *y*(*y), -2));
    *x *= c;
    *y *= c;
}

/* non utilisée */
void flame_random(double* x, double* y) // Cross (Variation 48)
{
    //algorithms[randinrange(0, FLAME_NUM_VARIATION-2)](x, y);
}
