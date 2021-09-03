/**
 * \file
 *         Header file for the blood pressure resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-blood-pressure Blood pressure resource
 * @{
 *
 * The res-blood-pressure module provides the implementation of a CoAP resource
 * representing the last blood pressure value sampled by the vital signs monitor.
 */

#ifndef SMART_ICU_RES_BLOOD_PRESSURE_H
#define SMART_ICU_RES_BLOOD_PRESSURE_H

/**
 * \brief   Activate the blood pressure resource.
 */
void res_blood_pressure_activate(void);

/**
 * \brief                  Update the blood pressure resource.
 * \param blood_pressure   The new blood pressure.
 *
 *                         This function updates the blood pressure resource,
 *                         triggering notifications to the observers.
 */
void res_blood_pressure_update(int blood_pressure);

#endif /* SMART_ICU_RES_BLOOD_PRESSURE_H */
/** @} */
