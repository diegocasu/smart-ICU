/**
 * \file
 *         Header file for the alarm system simulator
 * \author
 *         Diego Casu
 */

/**
 * \defgroup alarm Alarm system simulator
 * @{
 *
 * The alarm module provides a simulation of an alarm system characterized by a
 * state and an acoustic signal. When the system is turned on, the state switches to "ON" and
 * the signal is reproduced continuously for a given time. While the signal stops automatically,
 * the state of the alarm stays set to "ON" until an explicit reset command is issued.
 * The activation and deactivation of the alarm system is visually signaled by the activation and
 * deactivation of the monitor's LEDs.
 */

#ifndef SMART_ICU_ALARM_H
#define SMART_ICU_ALARM_H

#include "contiki.h"
#include "os/sys/ctimer.h"

/* Enumerator representing the alarm system state. */
typedef enum {
  ALARM_ON,
  ALARM_OFF,
} alarm_state;

/* Enumerator representing the acoustic signal state. */
typedef enum {
  ALARM_ACOUSTIC_SIGNAL_ON,
  ALARM_ACOUSTIC_SIGNAL_OFF,
} alarm_acoustic_signal_state;

/* Structure representing the alarm system. */
struct alarm_system {
  struct ctimer acoustic_timer;
  clock_time_t acoustic_signal_duration;
  alarm_state state;
  alarm_acoustic_signal_state acoustic_signal_state;
};

/**
 * \brief         Initialize the alarm system.
 * \param alarm   A pointer to the alarm system structure.
 *
 *                The function initializes the alarm system, setting <code>state</code>
 *                to ALARM_OFF and <code>acoustic_signal_state</code> to ALARM_ACOUSTIC_SIGNAL_OFF.
 */
void alarm_init(struct alarm_system *alarm);

/**
 * \brief         Turn on the alarm system.
 * \param alarm   A pointer to the alarm system structure.

 * \return        true if the alarm state was changed, false otherwise.
 *
 *                The function turns on the alarm system, setting <code>state</code>
 *                to ALARM_ON and <code>acoustic_signal_state</code> to ALARM_ACOUSTIC_SIGNAL_ON.
 *                The acoustic signal automatically stops after ALARM_SOUND_DURATION seconds.
 *                The return value tells if the alarm was off at the time of the function call.
 */
bool alarm_start(struct alarm_system *alarm);

/**
 * \brief         Turn off the alarm system.
 * \param alarm   A pointer to the alarm system structure.

 * \return        true if the alarm state was changed, false otherwise.
 *
 *                The function turns off the alarm system, setting <code>state</code>
 *                to ALARM_OFF and <code>acoustic_signal_state</code> to ALARM_ACOUSTIC_SIGNAL_OFF,
 *                eventually stopping the acoustic signal.
 *                The return value tells if the alarm was on at the time of the function call.
 */
bool alarm_stop(struct alarm_system *alarm);

#endif /* SMART_ICU_ALARM_H */
/** @} */
