#include "test.h"

/*
un des test les plus importants est la mesure précise du temps
*/

// exemple d'utilisation

/*/ enlever l'étoile pour utiliser
struct timespec t0, t1;
clock_gettime(CLOCK_MONOTONIC, &t0);//*/

// code à mesurer

/*/
clock_gettime(CLOCK_MONOTONIC, &t1);
double elapsed_time = timespecdiff(&t1, &t0);

printf("Executed in %.9fs\n", elapsed_time/1000000000);//*/

/**
Calcule la différence entre deux objets timespec

@param a timespec supérieur
@param b timespec inférieur
*/
double timespecdiff(struct timespec *a, struct timespec *b)
{
  return 1000000000*a->tv_sec + a->tv_nsec - 1000000000*b->tv_sec - b->tv_nsec;
}
