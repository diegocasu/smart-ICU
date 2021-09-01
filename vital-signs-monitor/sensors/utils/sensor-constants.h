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
#define HEART_RATE_SAMPLING_INTERVAL          10    //TODO: set correct values.
#define HEART_RATE_LOWER_BOUND                0     //TODO: set correct values.
#define HEART_RATE_UPPER_BOUND                300   //TODO: set correct values.
#define HEART_RATE_DEVIATION                  10    //TODO: set correct values.
#define HEART_RATE_UNIT                       "bpm"

/* Blood pressure sensor constants */
#define BLOOD_PRESSURE_SAMPLING_INTERVAL      11     //TODO: set correct values.
#define BLOOD_PRESSURE_LOWER_BOUND            10     //TODO: set correct values.
#define BLOOD_PRESSURE_UPPER_BOUND            10     //TODO: set correct values.
#define BLOOD_PRESSURE_DEVIATION              10     //TODO: set correct values.
#define BLOOD_PRESSURE_UNIT                   "mmHg"

/* Temperature sensor constants */
#define TEMPERATURE_SAMPLING_INTERVAL         22  //TODO: set correct values.
#define TEMPERATURE_LOWER_BOUND               10  //TODO: set correct values.
#define TEMPERATURE_UPPER_BOUND               10  //TODO: set correct values.
#define TEMPERATURE_DEVIATION                 10  //TODO: set correct values.
#define TEMPERATURE_UNIT                      "C" //TODO: set correct values.

/* Respiration sensor constants */
#define RESPIRATION_SAMPLING_INTERVAL         13    //TODO: set correct values.
#define RESPIRATION_LOWER_BOUND               10    //TODO: set correct values.
#define RESPIRATION_UPPER_BOUND               10    //TODO: set correct values.
#define RESPIRATION_DEVIATION                 10    //TODO: set correct values.
#define RESPIRATION_UNIT                      "bpm" //TODO: set correct values.

/* Oxygen saturation sensor constants */
#define OXYGEN_SATURATION_SAMPLING_INTERVAL   30  //TODO: set correct values.
#define OXYGEN_SATURATION_LOWER_BOUND         10  //TODO: set correct values.
#define OXYGEN_SATURATION_UPPER_BOUND         10  //TODO: set correct values.
#define OXYGEN_SATURATION_DEVIATION           10  //TODO: set correct values.
#define OXYGEN_SATURATION_UNIT                "%" //TODO: set correct values.

#endif /* SMART_ICU_SENSOR_CONSTANTS_H */
/** @} */
