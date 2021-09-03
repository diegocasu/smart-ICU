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

#include <stdlib.h>
#include "./sensor.h"

/*---------------------------------------------------------------------------*/
int
sensor_rand_int(int min, int max)
{
  return min + rand()/(RAND_MAX/(max - min + 1) + 1);
}
/*---------------------------------------------------------------------------*/
int
sensor_generate_sample(int starting_sample, int max_deviation, int lower_bound, int upper_bound)
{
  int deviation, new_sample;

  deviation = sensor_rand_int((-1)*max_deviation, max_deviation);
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
