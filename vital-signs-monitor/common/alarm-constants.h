/**
 * \file
 *         Constants used by the alarm system of a monitor
 * \author
 *         Diego Casu
 */

/**
 * \defgroup alarm-constants Alarm system constants
 * @{
 *
 * Constants used by the alarm system of a monitor to control the duration of acoustic signals
 * and to detect anomalous samples.
 */

#ifndef SMART_ICU_ALARM_CONSTANTS_H
#define SMART_ICU_ALARM_CONSTANTS_H

#define ALARM_SOUND_DURATION                    30  /* Duration in seconds of the alarm sound. */

#define ALARM_HEART_RATE_MIN_THRESHOLD          0   //TODO: set correct values.
#define ALARM_HEART_RATE_MAX_THRESHOLD          300 //TODO: set correct values.

#define ALARM_BLOOD_PRESSURE_MIN_THRESHOLD      0   //TODO: set correct values.
#define ALARM_BLOOD_PRESSURE_MAX_THRESHOLD      100 //TODO: set correct values.

#define ALARM_OXYGEN_SATURATION_MIN_THRESHOLD   90  //TODO: set correct values.
#define ALARM_OXYGEN_SATURATION_MAX_THRESHOLD   110 //TODO: set correct values.

#define ALARM_RESPIRATION_MIN_THRESHOLD         0   //TODO: set correct values.
#define ALARM_RESPIRATION_MAX_THRESHOLD         100 //TODO: set correct values.

#define ALARM_TEMPERATURE_MIN_THRESHOLD         32  //TODO: set correct values.
#define ALARM_TEMPERATURE_MAX_THRESHOLD         38  //TODO: set correct values.

#endif /* SMART_ICU_ALARM_CONSTANTS_H */
/** @} */
