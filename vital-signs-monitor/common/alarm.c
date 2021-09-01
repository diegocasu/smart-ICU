/**
 * \file
 *         Implementation of the alarm system simulator
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup alarm
 * @{
 */

#include "contiki.h"
#include "os/sys/log.h"
#include "os/dev/leds.h"
#include "./alarm.h"
#include "./alarm-constants.h"

#define LOG_MODULE "Alarm system"
#define LOG_LEVEL LOG_LEVEL_ALARM_SYSTEM

/*---------------------------------------------------------------------------*/
/* Callback function used by the ctimer. */
static void
stop_acoustic_signal(void *data)
{
  struct alarm_system *alarm = (struct alarm_system *)data;

  ctimer_stop(&alarm->acoustic_timer);
  alarm->acoustic_signal_state = ALARM_ACOUSTIC_SIGNAL_OFF;
  LOG_INFO("The acoustic signal of the alarm has been stopped.\n");
}
/*---------------------------------------------------------------------------*/
void
alarm_init(struct alarm_system *alarm)
{
  alarm->state = ALARM_OFF;
  alarm->acoustic_signal_state = ALARM_ACOUSTIC_SIGNAL_OFF;
  alarm->acoustic_signal_duration = ALARM_SOUND_DURATION*CLOCK_SECOND;
  leds_off(LEDS_ALL);
  LOG_INFO("Alarm system initialized.\n");
}
/*---------------------------------------------------------------------------*/
bool
alarm_start(struct alarm_system *alarm)
{
  if(alarm->state == ALARM_ON) {
    LOG_INFO("The alarm is already turned on. No actions will be performed.\n");
    return false;
  }

  alarm->state = ALARM_ON;
  leds_on(LEDS_ALL);

  /* Simulate an acoustic signal of a given duration. */
  alarm->acoustic_signal_state = ALARM_ACOUSTIC_SIGNAL_ON;
  ctimer_set(&alarm->acoustic_timer,
             alarm->acoustic_signal_duration,
             stop_acoustic_signal,
             (void *)alarm);

  LOG_INFO("The alarm has been turned on.\n");
  return true;
}
/*---------------------------------------------------------------------------*/
bool
alarm_stop(struct alarm_system *alarm)
{
  if(alarm->state == ALARM_OFF) {
    LOG_INFO("The alarm is already turned off. No actions will be performed.\n");
    return false;
  }

  alarm->state = ALARM_OFF;
  leds_off(LEDS_ALL);

  /* Stop the acoustic signal, if it has not been stopped yet. */
  alarm->acoustic_signal_state = ALARM_ACOUSTIC_SIGNAL_OFF;
  ctimer_stop(&alarm->acoustic_timer);

  LOG_INFO("The alarm has been turned off.\n");
  return true;
}
/*---------------------------------------------------------------------------*/
/** @} */
