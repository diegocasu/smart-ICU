/**
 * \file
 *         Header file for the respiration sensor simulator
 * \author
 *         Diego Casu
 */

/**
 * \defgroup respiration Respiration sensor simulator
 * @{
 *
 * The respiration module provides a simulation of the behaviour of a respiration sensor.
 * The sensor is implemented as a process that periodically generates a new sample within an interval
 * of possible values; the new sample is posted to a subscribed process.
 * The sampling can be started and stopped posting the associated events to the respiration process.
 */

#ifndef SMART_ICU_RESPIRATION_H
#define SMART_ICU_RESPIRATION_H

#include "contiki.h"

/* The process simulating a respiration sensor. */
PROCESS_NAME(respiration_sensor_process);

/*
 * Event that must be posted to respiration_sensor_process in order to start the sampling.
 * The additional data must carry a pointer to the process structure of the process
 * that will receive the samples. The latter is called the subscriber of the sensor.
 */
extern process_event_t RESPIRATION_START_SAMPLING_EVENT;

/* Event that must be sent to respiration_sensor_process in order to stop the sampling. */
extern process_event_t RESPIRATION_STOP_SAMPLING_EVENT;

/*
 * Event that respiration_sensor_process posts to the subscriber of the sensor to notify
 * the availability of a new sample. The sample, represented by a pointer to an int,
 * is posted as additional data.
 */
extern process_event_t RESPIRATION_SAMPLE_EVENT;

#endif /* SMART_ICU_RESPIRATION_H */
/** @} */
