#include "fconfig.h"
#include "flame.h"

/**
Ajoute une fractale à la liste principale des fractales, d'après l'emplacement d'un fichier de configuration
Cette fonction est récursive si le fichier de configuration est une liste de fichiers de configuration
*/
int load_fractal_config(int argc, char** argv)
{
    int series = 0;
    char** rec_argv = malloc(2*sizeof(char*));
    rec_argv[0] = malloc((strlen(argv[0]+1))*sizeof(char));
    strcpy(rec_argv[0], argv[0]);
    rec_argv[1] = malloc(4096*sizeof(char));
    char* base_path = malloc(4096*sizeof(char));
    int base_path_len = 0;

    int isset[FNUM_PARAM];
    int i;
    for(i = 0; i < FNUM_PARAM; i++)
        isset[FNUM_PARAM] = 0;

    fractal_t* f = malloc(sizeof(fractal_t));
    fractal_t* mbrot, * julia;
    FILE *ifp;
    char* line = malloc(4096*sizeof(char));
    char* buf = malloc(4096*sizeof(char));
    double lf1, lf2;
    double trc[10];
    unsigned int ui1, ui2;
    unsigned int tr_count = 0;
    unsigned int alg_count = 0;
    unsigned int max_size;
    ifs_tr_t* ifs_tr_list = malloc(1024*sizeof(ifs_tr_t));
    flame_tr_t* f_tr_list = malloc(1024*sizeof(flame_tr_t));
    unsigned int *flame_alg_id_list = malloc(1024*sizeof(unsigned int));
    for(i = 0; i < 1024; i++)
        flame_alg_id_list[i] = FLAME_RANDOM_ALGORITHM;

    if(argc != 2)
    {
        printf("Syntaxe : %s fichier_de_config\n", argv[0]);
        exit(0);
    }

    ifp = fopen(argv[1], "r");
    if(ifp == NULL){
      fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", argv[1]);
      exit(1);
    }

    f->width = 800;
    f->height = 800;
    f->scale = 1.0;
    f->iter = 255;
    f->position.x = 0.0;
    f->position.y = 0.0;

    while(fgets(line, 4095, ifp) != NULL)
    {
        strdown(line, 0, -1);
        if(strlen(line) < 2) continue;
        if(sscanf(line, "type %s", buf) == 1)
        {
            isset[FPARAM_TYPE]++;
            if(!strcmp(buf, "ifs"))
            {
                f->type = ft_ifs;
                if(!isset[FPARAM_NUM_ITER]) f->iter = 333;
            }
            else if(!strcmp(buf, "mandelbrot"))
            {
                f->type = ft_mandelbrot;
                if(!isset[FPARAM_POSITION]) f->position.x = -0.7;
            }
            else if(!strcmp(buf, "julia"))
            {
                f->type = ft_julia;
            }
            else if(!strcmp(buf, "flame"))
            {
                f->type = ft_flame;
                if(!isset[FPARAM_NUM_ITER]) f->iter = 1000;
                if(!isset[FPARAM_PROBA_TYPE]) f->param.flame.proba_type = fpt_set;
            }
            else if(!strcmp(buf, "series"))
            {
                while(fgets(line, 4000, ifp) != NULL)
                {
                    if(sscanf(line, "base_path %s", buf) == 1)
                    {
                        strcpy(base_path, buf);
                        base_path_len = strlen(base_path);
                        if(base_path[strlen(base_path)] != '/')
                        {
                            strcat(base_path, "/");
                            base_path_len++;
                        }
                    }
                    else
                    {
                        if(base_path_len)
                        {
                            strcat(base_path, line);
                            strcpy(rec_argv[1], base_path);
                            base_path[base_path_len] = 0;
                        }
                        else
                        {
                            strcpy(rec_argv[1], line);
                        }
                        rec_argv[1][strlen(rec_argv[1])-1] = 0;
                        if(rec_argv[1][strlen(rec_argv[1])-1] == '\r')
                            rec_argv[1][strlen(rec_argv[1])-1] = 0;
                        load_fractal_config(2, rec_argv);
                    }
                }
                series = 1;
                break;
            }
            else
            {
                f->type = ft_unknown;
                if(strcmp(buf, "interface"))
                    printf("Type de configuration inexistant : %s\n", line);
            }
        }
        else if(sscanf(line, "size %u %u", &ui1, &ui2) == 2)
        {
            isset[FPARAM_SIZE]++;
            f->width = ui1;
            f->height = ui2;
        }
        else if(sscanf(line, "scale %lf", &lf1) == 1)
        {
            isset[FPARAM_SCALE]++;
            f->scale = lf1;
        }
        else if(sscanf(line, "num_iter %u", &ui1) == 1)
        {
            isset[FPARAM_NUM_ITER]++;
            f->iter = ui1;
        }
        else if(sscanf(line, "position %lf %lf", &lf1, &lf2) == 2)
        {
            isset[FPARAM_POSITION]++;
            f->position.x = lf1;
            f->position.y = lf2;
        }
        else if(isset[FPARAM_TYPE])
        {
            if(f->type == ft_flame)
            {
                if(sscanf(line, "rand_transform %u %u", &ui1, &ui2) == 2)
                {
                    if(!isset[FPARAM_PROBA_TYPE]) f->param.flame.proba_type = fpt_random;
                    isset[FPARAM_RAND_TRANSFORM]++;
                    f->param.flame.min_num_tr = ui1;
                    f->param.flame.max_num_tr = ui2;
                    for(i = 0; i < ui2; i++)
                    {
                        f_tr_list[i].rand_tr = 1;
                        f_tr_list[i].rand_col = 1;
                    }
                }
                else if(sscanf(line, "rand_transform %u", &ui1) == 1)
                {
                    if(!isset[FPARAM_PROBA_TYPE]) f->param.flame.proba_type = fpt_random;
                    isset[FPARAM_RAND_TRANSFORM]++;
                    f->param.flame.min_num_tr = ui1;
                    f->param.flame.max_num_tr = ui1;
                    for(i = 0; i < ui1; i++)
                    {
                        f_tr_list[i].rand_tr = 1;
                        f_tr_list[i].rand_col = 1;
                    }
                }
                else if(sscanf(line, "rand_alg %u %u", &ui1, &ui2) == 2)
                {
                    isset[FPARAM_RAND_ALG]++;
                    f->param.flame.min_num_alg = ui1;
                    f->param.flame.max_num_alg = ui2;
                }
                else if(sscanf(line, "rand_alg %u", &ui1) == 1)
                {
                    isset[FPARAM_RAND_ALG]++;
                    f->param.flame.min_num_alg = ui1;
                    f->param.flame.max_num_alg = ui1;
                }
                else if(sscanf(line, "proba_type %s", buf) == 1)
                {
                    isset[FPARAM_PROBA_TYPE]++;
                    if(!strcmp(buf, "uniform"))
                    {
                        f->param.flame.proba_type = fpt_uniform;
                    }
                    else
                    {
                        f->param.flame.proba_type = fpt_random;
                        if(strcmp(buf, "random"))
                            printf("Type de configuration de probabilités inexistant : %s\n", line);
                    }
                }
                else if(sscanf(line,
                    "add_flame_transform %lf %lf %lf %lf %lf %lf %lf, %lf %lf %lf",
                    &trc[0], &trc[1], &trc[2], &trc[3], &trc[4],
                    &trc[5], &trc[6], &trc[7], &trc[8], &trc[9]) == 10)
                {
                    f_tr_list[tr_count].rand_tr = 0;
                    f_tr_list[tr_count].rand_col = 0;
                    for(i = 0; i < 10; i++)
                        f_tr_list[tr_count].tr[i] = trc[i];
                    tr_count++;
                    f->param.flame.min_num_tr = tr_count;
                    f->param.flame.max_num_tr = tr_count;
                    f->param.flame.num_tr = tr_count;
                }
                else if(sscanf(line,
                    "add_flame_transform random, %lf %lf %lf",
                    &trc[0], &trc[1], &trc[2]) == 3)
                {
                    f_tr_list[tr_count].rand_tr = 1;
                    f_tr_list[tr_count].rand_col = 0;
                    for(i = 7; i < 10; i++)
                        f_tr_list[tr_count].tr[i] = trc[i];
                    tr_count++;
                    f->param.flame.min_num_tr = tr_count;
                    f->param.flame.max_num_tr = tr_count;
                    f->param.flame.num_tr = tr_count;
                }
                else if(sscanf(line,
                    "add_flame_transform %lf %lf %lf %lf %lf %lf %lf, random",
                    &trc[0], &trc[1], &trc[2], &trc[3], &trc[4],
                    &trc[5], &trc[6]) == 7)
                {
                    f_tr_list[tr_count].rand_tr = 0;
                    f_tr_list[tr_count].rand_col = 1;
                    for(i = 0; i < 7; i++)
                        f_tr_list[tr_count].tr[i] = trc[i];
                    tr_count++;
                    f->param.flame.min_num_tr = tr_count;
                    f->param.flame.max_num_tr = tr_count;
                    f->param.flame.num_tr = tr_count;
                }
                else if(!strcmp(line, "add_flame_transform random, random"))
                {
                    f_tr_list[tr_count].rand_tr = 1;
                    f_tr_list[tr_count].rand_col = 1;
                    tr_count++;
                    f->param.flame.min_num_tr = tr_count;
                    f->param.flame.max_num_tr = tr_count;
                    f->param.flame.num_tr = tr_count;
                }
                else if(sscanf(line, "add_algorithm %s", buf))
                {
                    flame_alg_id_list[alg_count] = flame_alg_by_name(buf);
                    alg_count++;
                    f->param.flame.min_num_alg = alg_count;
                    f->param.flame.max_num_alg = alg_count;
                    f->param.flame.num_alg = alg_count;
                }
                else
                {
                    printf("Flamme : ligne ignorée : %s", line);
                }
            }
            else if(f->type == ft_ifs)
            {
                if(sscanf(line,
                    "add_transform %lf %lf %lf %lf %lf %lf %lf",
                    &trc[0], &trc[1], &trc[2], &trc[3], &trc[4],
                    &trc[5], &trc[6]) == 7)
                {
                    for(i = 0; i < 7; i++)
                        ifs_tr_list[tr_count].tr[i] = trc[i];
                    tr_count++;
                }
                else
                {
                    printf("IFS : ligne ignorée : %s", line);
                }
            }
            else if(f->type == ft_julia)
            {
                if(sscanf(line, "julia.c %lf %lf", &lf1, &lf2) == 2)
                {
                    f->param.julia.c.x = lf1;
                    f->param.julia.c.y = lf2;
                }
                else
                {
                    printf("IFS : ligne ignorée : %s", line);
                }
            }
        }
        else
        {
            printf("Configuration : Ligne ignorée : %s\n", line);
        }
    }

    if(f->iter == 0 && (f->type == ft_ifs || f->type == ft_flame))
    {
        max_size = f->width > f->height ? f->width : f->height;
        max_size *= max_size*max_size;
        f->iter = max_size < 8000000 ? max_size : 8000000;
    }

    if(f->type == ft_flame)
    {
        f->param.flame.tr = malloc(f->param.flame.max_num_tr*sizeof(flame_tr_t));
        for(i = 0; i < f->param.flame.max_num_tr; i++)
            f->param.flame.tr[i] = f_tr_list[i];

        f->param.flame.alg_id_list =
            malloc(f->param.flame.max_num_alg*sizeof(unsigned int));
        for(i = 0; i < f->param.flame.max_num_alg; i++)
            f->param.flame.alg_id_list[i] = flame_alg_id_list[i];

        flame_init(f);
    }
    else if(f->type == ft_ifs)
    {
        f->param.ifs.num_tr = tr_count;
        f->param.ifs.tr = malloc(tr_count*sizeof(ifs_tr_t));
        for(i = 0; i < tr_count; i++)
            f->param.ifs.tr[i] = ifs_tr_list[i];
    }
    else if(f->type == ft_mandelbrot)
    {
        julia = malloc(sizeof(fractal_t));
        f->param.mbrot.julia = julia;
        julia->param.julia.mbrot = f;
        julia->width = f->width;
        julia->width = f->height;
        julia->type = ft_julia;
        julia->position.x = 0.0;
        julia->position.y = 0.0;
        julia->param.julia.c.x = 0.0;
        julia->param.julia.c.y = 0.0;
        julia->iter = 255;
        julia->scale = 1.0;
    }
    else if(f->type == ft_julia)
    {
        mbrot = malloc(sizeof(fractal_t));
        f->param.julia.mbrot = mbrot;
        mbrot->param.mbrot.julia = f;
        mbrot->width = f->width;
        mbrot->height = f->height;
        mbrot->type = ft_mandelbrot;
        mbrot->position.x = -0.7;
        mbrot->position.y = 0.0;
        mbrot->iter = 255;
        mbrot->scale = 1.0/1.5;
    }

    if(f->type != ft_unknown && !series)
    {
        add_fractal(f);
        set_current_fractal(fractal_list[0]);
    }

    free(buf);
    free(line);
    free(ifs_tr_list);
    free(f_tr_list);
    free(flame_alg_id_list);
    free(base_path);
    free(rec_argv[0]);
    free(rec_argv[1]);
    free(rec_argv);

    return 0;
}

/**
Diminue la casse sur une partie de la chaîne
@param s chaîne à transformer
@param b début de la substitution (Comme dans le language Ruby : 0 pour le début de la chaîne, -1 pour la fin, n pour le nième, -n-1 pour le nième depuis la fin)
@param e fin de la substitution (sa valeur dans l'index de la chaîne doit être supérieure)
*/
void strdown(char* s, int b, int e)
{
    int size, i;
    if(b < 0)
    {
        if(e < 0)
        {
            size = strlen(s);
            e += size; // Ruby style
            b += size;
        }
        else b += strlen(s);
    }
    else if(e < 0) e += strlen(s);
    for(i = b; i <= e; i++) s[i] = tolower(s[i]);
}

fractal_t* current_fractal_system = NULL;

/**
Fractale qui est le centre d'intérêt du moment du programme
*/
fractal_t* current_fractal()
{
    return current_fractal_system;
}

void set_current_fractal(fractal_t* f)
{
    current_fractal_system = f;
}

fractal_t** fractal_list = NULL;
int fractal_list_size = 0;
int current_fractal_index = 0;

/**
Passe le focus à la fractale précédente (dans la liste principale des fractales)
Avant la première vient la dernière
*/
void previous_fractal()
{
    current_fractal_index += fractal_list_size-1;
    current_fractal_system = fractal_list[current_fractal_index %= fractal_list_size];
}

/**
Passe le focus à la fractale suivante (dans la liste principale des fractales)
Après la dernière vient la première
*/
void next_fractal()
{
    current_fractal_index++;
    current_fractal_system = fractal_list[current_fractal_index %= fractal_list_size];
}

/**
Ajoute une fractale à la liste principale des fractales
*/
void add_fractal(fractal_t* f)
{
    if(!++fractal_list_size)
    {
        fractal_list = malloc(sizeof(fractal_t*));
    }
    else
    {
        fractal_list = realloc(fractal_list, fractal_list_size*sizeof(fractal_t*));
    }
    fractal_list[fractal_list_size-1] = f;
}

/**
Libère toute la mémoire utilisée par la fractale
*/
void free_fractal(fractal_t* f){
    switch(f->type)
    {
        case ft_mandelbrot:
            free(f->param.mbrot.julia);
        break;
        case ft_julia:
            free(f->param.julia.mbrot);
        break;
        case ft_ifs:
            free(f->param.ifs.tr);
        break;
        case ft_flame:
            free(f->param.flame.tr);
            free(f->param.flame.alg_id_list);
            free(f->param.flame.alg_id_list2);
            free(f->param.flame.dots);
        break;
        default:
        break;
    }
    free(f);
}
