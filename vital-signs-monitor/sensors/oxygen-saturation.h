/**
 * \file
 *         Header file for the oxygen saturation sensor simulator
 * \author
 *         Diego Casu
 */

/**
 * \defgroup oxygen-saturation Oxygen saturation sensor simulator
 * @{
 *
 * The oxygen-saturation module provides a simulation of the behaviour of an oxygen saturation sensor.
 * The sensor is implemented as a process that periodically generates a new sample within an interval
 * of possible values; the new sample is posted to a subscribed process.
 * The sampling can be started and stopped posting the associated events to the oxygen saturation process.
 */

#ifndef SMART_ICU_OXYGEN_SATURATION_H
#define SMART_ICU_OXYGEN_SATURATION_H

#include "contiki.h"

/* The process simulating an oxygen saturation sensor. */
PROCESS_NAME(oxygen_saturation_sensor_process);

/*
 * Event that must be posted to oxygen_saturation_sensor_process in order to start the sampling.
 * The additional data must carry a pointer to the process structure of the process
 * that will receive the samples. The latter is called the subscriber of the sensor.
 */
extern process_event_t OXYGEN_SATURATION_START_SAMPLING_EVENT;

/* Event that must be sent to oxygen_saturation_sensor_process in order to stop the sampling. */
extern process_event_t OXYGEN_SATURATION_STOP_SAMPLING_EVENT;

/*
 * Event that oxygen_saturation_sensor_process posts to the subscriber of the sensor to notify
 * the availability of a new sample. The sample, represented by a pointer to an int,
 * is posted as additional data.
 */
extern process_event_t OXYGEN_SATURATION_SAMPLE_EVENT;

#endif //SMART_ICU_OXYGEN_SATURATION_H
/** @} */
