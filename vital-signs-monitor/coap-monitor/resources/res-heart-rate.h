/**
 * \file
 *         Header file for the heart rate resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-heart-rate Heart rate resource
 * @{
 *
 * The res-heart-rate module provides the implementation of a CoAP resource
 * representing the last heart rate value sampled by the vital signs monitor.
 */

#ifndef SMART_ICU_RES_HEART_RATE_H
#define SMART_ICU_RES_HEART_RATE_H

/**
 * \brief   Activate the heart rate resource.
 */
void res_heart_rate_activate(void);

/**
 * \brief              Update the heart rate resource.
 * \param heart_rate   The new heart rate.
 *
 *                     This function updates the heart rate resource,
 *                     triggering notifications to the observers.
 */
void res_heart_rate_update(int heart_rate);

#endif /* SMART_ICU_RES_HEART_RATE_H */
/** @} */
