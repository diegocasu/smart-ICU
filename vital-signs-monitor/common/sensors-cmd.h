/**
 * \file
 *         Header file for the utility functions to manage sensors
 * \author
 *         Diego Casu
 */

/**
 * \defgroup sensors-cmd Utility functions to manage sensors
 * @{
 *
 * The sensors-cmd module provides a set of utility functions to manage the sensor processes.
 * The functions allow to start/stop the processes and to start/stop their sampling activity;
 * moreover, they allow to check if a sample event is coming from a certain type of sensor.
 */

#ifndef SMART_ICU_SENSORS_CMD_H
#define SMART_ICU_SENSORS_CMD_H

#include "contiki.h"
#include <stdbool.h>

/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by the heart rate sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
bool sensors_cmd_heart_rate_sample_event(process_event_t event);

/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by the blood pressure sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
bool sensors_cmd_blood_pressure_sample_event(process_event_t event);

/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by the oxygen saturation sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
bool sensors_cmd_oxygen_saturation_sample_event(process_event_t event);

/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by the respiration sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
bool sensors_cmd_respiration_sample_event(process_event_t event);

/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by the temperature sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
bool sensors_cmd_temperature_sample_event(process_event_t event);

/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by a sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
bool sensors_cmd_sample_event(process_event_t event);

/**
 * \brief   Start the processes simulating the sensors.
 */
void sensors_cmd_start_processes(void);

/**
 * \brief                       Start the sampling activity of the processes simulating the sensors.
 * \param subscribing_process   The process that will receive notifications about
 *                              the availability of new samples.
 */
void sensors_cmd_start_sampling(process_data_t subscribing_process);

/**
 * \brief   Stop the sampling activity of the processes simulating the sensors.
 */
void sensors_cmd_stop_sampling(void);

/**
 * \brief   Stop the processes simulating the sensors.
 */
void sensors_cmd_stop_processes(void);

#endif /* SMART_ICU_SENSORS_CMD_H */
/** @} */
