/**
 * \file
 *         Header file for the alarm state resource
 * \author
 *         Diego Casu
 */

/**
 * \defgroup res-alarm-state Alarm state resource
 * @{
 *
 * The res-alarm-state module provides the implementation of a CoAP resource
 * representing the current alarm state of the vital signs monitor.
 */

#include <stdbool.h>
#include "../../common/alarm.h"

#ifndef SMART_ICU_RES_ALARM_STATE_H
#define SMART_ICU_RES_ALARM_STATE_H

/**
 * \brief         Activate the alarm state resource.
 * \param alarm   A pointer to the alarm system.
 *
 *                This function activates the alarm state resource.
 *                The pointer to the alarm system is needed
 *                to ensure that a PUT targeting this resource
 *                turns on the alarm.
 */
void res_alarm_state_activate(struct alarm_system *alarm);

/**
 * \brief               Update the alarm state resource.
 * \param alarm_state   The new alarm state.
 *
 *                      This function updates the alarm state resource,
 *                      triggering notifications to the observers.
 */
void res_alarm_state_update(alarm_state alarm_state);

#endif /* SMART_ICU_RES_ALARM_STATE_H */
/** @} */
