/**
 * \file
 *         PRNG implementation
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup prng
 * @{
 */

#include <stdlib.h>
#include "./prng.h"

/*---------------------------------------------------------------------------*/
void
prng_init(unsigned int seed)
{
  srand(seed);
}
/*---------------------------------------------------------------------------*/
int
prng_rand(int min, int max)
{
  return min + rand()/(RAND_MAX/(max - min + 1) + 1);
}
/*---------------------------------------------------------------------------*/
/** @} */
