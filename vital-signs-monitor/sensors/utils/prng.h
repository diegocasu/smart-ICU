/**
 * \file
 *         Header file for the PRNG used by the simulated sensors
 * \author
 *         Diego Casu
 */

/**
 * \defgroup prng Pseudorandom number generator
 * @{
 *
 * The prng module offers a pseudorandom number generator based on the srand() and rand() functions
 * provided by the standard library. The module allows to seed the PRNG and extract a number inside
 * a given interval.
 */

#ifndef SMART_ICU_PRNG_H
#define SMART_ICU_PRNG_H

/**
 * \brief        Initialize the PRNG with a seed.
 * \param seed   The seed for the PRNG.
 *
 *               The function initializes the PRNG with a given seed.
 *               The implementation is a call to the srand() function provided by the standard library.
 *               This function should be called only one time.
 */
void prng_init(unsigned int seed);

/**
 * \brief       Extract a pseudorandom integer number in the interval [min, max].
 * \param min   The lower bound (inclusive) of the interval.
 * \param max   The upper bound (inclusive) of the interval.
 *
 *              The function extracts a pseudorandom integer number in the interval [min, max].
 *              The implementation is based on the rand() function provided by the standard library.
 */
int prng_rand(int min, int max);

#endif //SMART_ICU_PRNG_H
/** @} */

