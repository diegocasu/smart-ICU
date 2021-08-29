/**
 * \file
 *         Functions generating samples for generic simulated sensors
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup sensor
 * @{
 */

#include "./sensor.h"
#include "./utils/prng.h"

/*---------------------------------------------------------------------------*/
int
sensor_generate_sample(int starting_sample, int max_deviation, int lower_bound, int upper_bound) {
  int deviation, new_sample;

  deviation = prng_rand((-1)*max_deviation, max_deviation);
  new_sample = starting_sample + deviation;

  if(new_sample < lower_bound) {
    return lower_bound;
  }

  if(new_sample > upper_bound) {
    return upper_bound;
  }

  return new_sample;
}
/*---------------------------------------------------------------------------*/
/** @} */
