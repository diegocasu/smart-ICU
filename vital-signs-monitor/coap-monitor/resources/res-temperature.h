/**
 * \file
 *         Header file for the temperature resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-temperature Temperature resource
 * @{
 *
 * The res-temperature module provides the implementation of a CoAP resource
 * representing the last temperature value sampled by the vital signs monitor.
 */

#ifndef SMART_ICU_RES_TEMPERATURE_H
#define SMART_ICU_RES_TEMPERATURE_H

/**
 * \brief   Activate the temperature resource.
 */
void res_temperature_activate(void);

/**
 * \brief               Update the temperature resource.
 * \param temperature   The new temperature.
 *
 *                      This function updates the temperature resource,
 *                      triggering notifications to the observers.
 */
void res_temperature_update(int temperature);

#endif /* SMART_ICU_RES_TEMPERATURE_H */
/** @} */
