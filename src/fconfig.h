#ifndef FCONFIG_H_INCLUDED
#define FCONFIG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <SDL/SDL.h>
#include "def.h"

#define FNUM_PARAM 8
#define FPARAM_TYPE 0
#define FPARAM_SIZE 1
#define FPARAM_SCALE 2
#define FPARAM_NUM_ITER 3
#define FPARAM_POSITION 4
#define FPARAM_RAND_TRANSFORM 5
#define FPARAM_RAND_ALG 6
#define FPARAM_PROBA_TYPE 7

/*
on défini un vecteur du plan (R2) des réels
*/

typedef struct
{
    double x, y;
} R2vec;

typedef enum FRACTAL_TYPE
{
    ft_mandelbrot,
    ft_julia,
    ft_ifs,
    ft_flame,
    ft_unknown
} fractal_type_t;

typedef enum FLAME_PROBA_TYPE
{
    fpt_set,
    fpt_uniform,
    fpt_random
} proba_type_t;

/*
Le type de la fractale retournée par current_fractal() et celui de la fractale affichée en grand sur l'écran.
Si ce type est Mandelbrot, une miniature de la fractale de Julia correspondant à l'emplacement de la souris.
Un clique permet d'afficher la fractale de Julia en grand.
*/
struct fractal
{
    enum FRACTAL_TYPE type; // le type de fractale
    unsigned int width, height; // dimensions d'affichage en pixels
    double scale; // l'échelle d'affichage (ou zoom)

    /*
    le nombre maximum d'itérations par pixel pour les fractales à temps d'échappement
    ou bien le nombre d'itérations pour les IFS
    */
    unsigned int iter;

    /*
    le décalage de la caméra par rapport à l'origine
    */
    R2vec position;

    /*
    paramètres de la fractale (dépend de son type)
    */
    union
    {
        /*
        pour l'ensemble de Mandelbrot
        */
        struct
        {
            /*
            un lien vers la fractale de Julia dont la constante correspond à l'emplacement de la souris sur l'ensemble de Mandelbrot
            ce lien doit être défini et réciproque :
                mbrot_set->param.mbrot.julia->param.julia.mbrot == mbrot_set
            */
            struct fractal* julia;
        } mbrot;

        /*
        pour une fractale de Julia
        */
        struct
        {
            /*
            constante de Julia
            */
            R2vec c;

            struct fractal* mbrot; // le lien réciproque vers l'ensemble de Mandelbrot
        } julia;

        /*
        pour toutes les IFS (Iterated Function System)
        */
        struct
        {
            unsigned int num_tr; // nombre de transformations
            struct ifs_tr
            {
                union // permet de d'utilise un tableau à la place de la structure
                {
                    double tr[7];
                    struct
                    {
                        double xx, xy, x, yx, yy, y, proba;
                    };
                };
            }* tr; // liste des transformations
        } ifs;

        struct
        {
            enum FLAME_PROBA_TYPE proba_type;
            unsigned int min_num_tr; // min et max peuvent être égaux
            unsigned int max_num_tr; // à utiliser pour l'allocation mémoire
            unsigned int num_tr; // choisi aléatoirement entre min et max (à chaque génération)
            struct flame_tr
            {
                int rand_col; // est-ce que les coefficients initiaux des couleurs doivent être générés aléatoirement ?
                int rand_tr; // et pour les coefficients de la transformation ?
                union
                {
                    double tr[10];
                    struct
                    {
                        double xx, xy, x, yx, yy, y, proba, r, g, b;
                    };
                };
            }* tr;
            unsigned int min_num_alg, max_num_alg, num_alg;
            unsigned int *alg_id_list; // liste des identifiants des algorithmes à utiliser
            unsigned int *alg_id_list2; // ne contient pas l'idendifiant aléatoire
            unsigned int current_iter; // iteration en cours
            struct flame_dot_t
            {
                R2vec position;
                Uint8 color[3];
            }* dots; // sauvegarde des données sur les points générés
            Uint8 current_color[3];
            R2vec current_position;
        } flame;
    } param;
};

typedef struct fractal fractal_t;
typedef struct ifs_tr ifs_tr_t;
typedef struct flame_tr flame_tr_t;
typedef struct flame_dot_t flame_dot_t;

int load_fractal_config(int argc, char** argv);

void strdown(char* s, int b, int e);

extern fractal_t* current_fractal_system;

fractal_t* current_fractal();

void set_current_fractal(fractal_t* f);

extern fractal_t** fractal_list;
extern int fractal_list_size;
extern int current_fractal_index;

void previous_fractal();
void next_fractal();

void add_fractal(fractal_t* f);

void free_fractal(fractal_t* f);

#endif // FCONFIG_H_INCLUDED
