/**
 * \file
 *         Constants used by the simulated sensors
 * \author
 *         Diego Casu
 */

/**
 * \defgroup sensor-constants Sensor constants
 * @{
 *
 * Constants used by the processes simulating the heart rate, blood pressure, temperature
 * respiration and oxygen saturation sensors.
 */

#ifndef SMART_ICU_SENSOR_CONSTANTS_H
#define SMART_ICU_SENSOR_CONSTANTS_H

/* Heart rate sensor constants */
#define HEART_RATE_SAMPLING_INTERVAL          60
#define HEART_RATE_LOWER_BOUND                0
#define HEART_RATE_UPPER_BOUND                170
#define HEART_RATE_DEVIATION                  10
#define HEART_RATE_UNIT                       "bpm"

/* Blood pressure sensor constants */
#define BLOOD_PRESSURE_SAMPLING_INTERVAL      120
#define BLOOD_PRESSURE_LOWER_BOUND            60
#define BLOOD_PRESSURE_UPPER_BOUND            200
#define BLOOD_PRESSURE_DEVIATION              5
#define BLOOD_PRESSURE_UNIT                   "mmHg"

/* Temperature sensor constants */
#define TEMPERATURE_SAMPLING_INTERVAL         180
#define TEMPERATURE_LOWER_BOUND               30
#define TEMPERATURE_UPPER_BOUND               45
#define TEMPERATURE_DEVIATION                 2
#define TEMPERATURE_UNIT                      "C"

/* Respiration sensor constants */
#define RESPIRATION_SAMPLING_INTERVAL         60
#define RESPIRATION_LOWER_BOUND               8
#define RESPIRATION_UPPER_BOUND               30
#define RESPIRATION_DEVIATION                 2
#define RESPIRATION_UNIT                      "bpm"

/* Oxygen saturation sensor constants */
#define OXYGEN_SATURATION_SAMPLING_INTERVAL   120
#define OXYGEN_SATURATION_LOWER_BOUND         80
#define OXYGEN_SATURATION_UPPER_BOUND         100
#define OXYGEN_SATURATION_DEVIATION           5
#define OXYGEN_SATURATION_UNIT                "%"

#endif /* SMART_ICU_SENSOR_CONSTANTS_H */
/** @} */
