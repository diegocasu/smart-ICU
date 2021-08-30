/**
 * \file
 *         Header file for the blood pressure sensor simulator
 * \author
 *         Diego Casu
 */

/**
 * \defgroup blood-pressure Blood pressure sensor simulator
 * @{
 *
 * The blood-pressure module provides a simulation of the behaviour of a blood pressure sensor.
 * The sensor is implemented as a process that periodically generates a new sample within an interval
 * of possible values; the new sample is posted to a subscribed process.
 * The sampling can be started and stopped posting the associated events to the blood pressure process.
 */

#ifndef SMART_ICU_BLOOD_PRESSURE_H
#define SMART_ICU_BLOOD_PRESSURE_H

#include "contiki.h"

/* The process simulating a blood pressure sensor. */
PROCESS_NAME(blood_pressure_sensor_process);

/*
 * Event that must be posted to blood_pressure_sensor_process in order to start the sampling.
 * The additional data must carry a pointer to the process structure of the process
 * that will receive the samples. The latter is called the subscriber of the sensor.
 */
extern process_event_t BLOOD_PRESSURE_START_SAMPLING_EVENT;

/* Event that must be sent to blood_pressure_sensor_process in order to stop the sampling. */
extern process_event_t BLOOD_PRESSURE_STOP_SAMPLING_EVENT;

/*
 * Event that blood_pressure_sensor_process posts to the subscriber of the sensor to notify
 * the availability of a new sample. The sample, represented by a pointer to an int,
 * is posted as additional data.
 */
extern process_event_t BLOOD_PRESSURE_SAMPLE_EVENT;

#endif /* SMART_ICU_BLOOD_PRESSURE_H */
/** @} */