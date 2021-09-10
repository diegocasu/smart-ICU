/**
 * \file
 *         Header file for a generic simulated sensor
 * \author
 *         Diego Casu
 */

/**
 * \defgroup sensor Generic simulated sensor
 * @{
 *
 * The sensor module provides a representation of generic simulated sensors inside a
 * smart ICU monitor and functions to generate new samples inside given intervals.
 * A generic sensor implementation must define a process simulating the sampling activity;
 * in particular, this process must:<br>
 * 1) start generating samples periodically when it receives a start event.
 *    The event is posted by another process, so that a process structure, identifying the
 *    subscribed process that will receive the samples, is attached as additional data.
 *    Each time a new sample is generated, it is saved in the dedicated field of the sensor
 *    and posted to the subscriber.<br>
 * 2) stop generating samples when it receives a stop event.<br>
 * The start, stop and new sample events must be defined in the implementation of the sensor.
 */

#ifndef SMART_ICU_SENSOR_H
#define SMART_ICU_SENSOR_H

#include "contiki.h"
#include "sys/etimer.h"

/* Structure representing a generic sensor inside a smart ICU monitor. */
struct sensor {
  struct etimer sampling_timer;
  clock_time_t sampling_interval;
  int last_sample;
  struct process *subscriber;
};

/**
 * \brief       Extract a pseudorandom integer number in the interval [min, max].
 * \param min   The lower bound (inclusive) of the interval.
 * \param max   The upper bound (inclusive) of the interval.
 * \return      A a pseudorandom integer number in the interval [min, max].
 *
 *              The function extracts a pseudorandom integer number in the interval [min, max].
 *              The implementation is based on the <code>rand()</code> function provided
 *              by the standard library.
 */
int sensor_rand_int(int min, int max);

/**
 * \brief                   Generate a new integer sample in the interval [<i>lower_bound</i>, <i>upper_bound</i>],
 *                          starting from a previous value.
 * \param starting_sample   The sample from which the new one will be derived.
 * \param max_deviation     The maximum deviation used to generate the new sample.
 * \param lower_bound       The lower bound (inclusive) of the interval.
 * \param upper_bound       The upper bound (inclusive) of the interval.
 * \return                  A new integer sample in the interval [<i>lower_bound</i>, <i>upper_bound</i>].
 *
 *                          Generate a new integer sample in the interval [<i>lower_bound</i>, <i>upper_bound</i>],
 *                          starting from a previous value.
 *                          A new sample is derived from the last one by adding or subtracting a deviation,
 *                          respecting the upper and lower bounds of the interval.
 *                          The deviation is a random number in the interval [<i>-max_deviation</i>, <i>max_deviation</i>].
 */
int sensor_generate_sample(int starting_sample, int max_deviation, int lower_bound, int upper_bound);

#endif /* SMART_ICU_SENSOR_H */
/** @} */
