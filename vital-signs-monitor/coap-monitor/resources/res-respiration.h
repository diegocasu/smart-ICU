/**
 * \file
 *         Header file for the respiration resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-respiration Respiration resource
 * @{
 *
 * The res-respiration module provides the implementation of a CoAP resource
 * representing the last respiration value sampled by the vital signs monitor.
 */

#ifndef SMART_ICU_RES_RESPIRATION_H
#define SMART_ICU_RES_RESPIRATION_H

/**
 * \brief   Activate the respiration resource.
 */
void res_respiration_activate(void);

/**
 * \brief               Update the respiration resource.
 * \param respiration   The new respiration.
 *
 *                      This function updates the respiration resource,
 *                      triggering notifications to the observers.
 */
void res_respiration_update(int respiration);

#endif /* SMART_ICU_RES_RESPIRATION_H */
/** @} */
