/**
 * \file
 *         Header file for the oxygen saturation resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-oxygen-saturation Oxygen saturation resource
 * @{
 *
 * The res-oxygen-saturation module provides the implementation of a CoAP resource
 * representing the last oxygen saturation value sampled by the vital signs monitor.
 */

#ifndef SMART_ICU_RES_OXYGEN_SATURATION_H
#define SMART_ICU_RES_OXYGEN_SATURATION_H

/**
 * \brief   Activate the oxygen saturation resource.
 */
void res_oxygen_saturation_activate(void);

/**
 * \brief                     Update the oxygen saturation resource.
 * \param oxygen_saturation   The new oxygen saturation.
 *
 *                            This function updates the oxygen saturation resource,
 *                            triggering notifications to the observers.
 */
void res_oxygen_saturation_update(int oxygen_saturation);

#endif /* SMART_ICU_RES_OXYGEN_SATURATION_H */
/** @} */
