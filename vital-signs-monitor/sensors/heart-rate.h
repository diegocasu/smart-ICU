/**
 * \file
 *         Header file for the heart rate sensor simulator
 * \author
 *         Diego Casu
 */

/**
 * \defgroup heart-rate Heart rate sensor simulator
 * @{
 *
 * The heart-rate module provides a simulation of the behaviour of a heart rate sensor.
 * The sensor is implemented as a process that periodically generates a new sample within an interval
 * of possible values; the new sample is posted to a subscribed process.
 * The sampling can be started and stopped posting the associated events to the heart rate process.
 */

#ifndef SMART_ICU_HEART_RATE_H
#define SMART_ICU_HEART_RATE_H

#include "contiki.h"

/* The process simulating a heart rate sensor. */
PROCESS_NAME(heart_rate_sensor_process);

/*
 * Event that must be posted to heart_rate_sensor_process in order to start the sampling.
 * The additional data must carry a pointer to the process structure of the process
 * that will receive the samples. The latter is called the subscriber of the sensor.
 */
extern process_event_t HEART_RATE_START_SAMPLING_EVENT;

/* Event that must be sent to heart_rate_sensor_process in order to stop the sampling. */
extern process_event_t HEART_RATE_STOP_SAMPLING_EVENT;

/*
 * Event that heart_rate_sensor_process posts to the subscriber of the sensor to notify
 * the availability of a new sample. The sample, represented by a pointer to an int,
 * is posted as additional data.
 */
extern process_event_t HEART_RATE_SAMPLE_EVENT;

#endif //SMART_ICU_HEART_RATE_H
/** @} */
