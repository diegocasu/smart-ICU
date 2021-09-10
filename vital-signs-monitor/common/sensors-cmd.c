/**
 * \file
 *         Implementation of the utility functions to manage sensors
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup sensors-cmd
 * @{
 */

#include "contiki.h"
#include "./sensors/heart-rate.h"
#include "./sensors/blood-pressure.h"
#include "./sensors/temperature.h"
#include "./sensors/respiration.h"
#include "./sensors/oxygen-saturation.h"
#include "./sensors-cmd.h"

/*---------------------------------------------------------------------------*/
bool
sensors_cmd_heart_rate_sample_event(process_event_t event)
{
  if(event == HEART_RATE_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
bool
sensors_cmd_blood_pressure_sample_event(process_event_t event)
{
  if(event == BLOOD_PRESSURE_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
bool
sensors_cmd_oxygen_saturation_sample_event(process_event_t event)
{
  if(event == OXYGEN_SATURATION_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
bool
sensors_cmd_respiration_sample_event(process_event_t event)
{
  if(event == RESPIRATION_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
bool
sensors_cmd_temperature_sample_event(process_event_t event)
{
  if(event == TEMPERATURE_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
bool
sensors_cmd_sample_event(process_event_t event)
{
  if(sensors_cmd_heart_rate_sample_event(event)
     || sensors_cmd_blood_pressure_sample_event(event)
     || sensors_cmd_oxygen_saturation_sample_event(event)
     || sensors_cmd_respiration_sample_event(event)
     || sensors_cmd_temperature_sample_event(event)) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
void
sensors_cmd_start_processes(void)
{
  process_start(&heart_rate_sensor_process, NULL);
  process_start(&blood_pressure_sensor_process, NULL);
  process_start(&temperature_sensor_process, NULL);
  process_start(&respiration_sensor_process, NULL);
  process_start(&oxygen_saturation_sensor_process, NULL);
}
/*---------------------------------------------------------------------------*/
void
sensors_cmd_start_sampling(process_data_t subscribing_process)
{
  process_post(&heart_rate_sensor_process, HEART_RATE_START_SAMPLING_EVENT, subscribing_process);
  process_post(&blood_pressure_sensor_process, BLOOD_PRESSURE_START_SAMPLING_EVENT, subscribing_process);
  process_post(&temperature_sensor_process, TEMPERATURE_START_SAMPLING_EVENT, subscribing_process);
  process_post(&respiration_sensor_process, RESPIRATION_START_SAMPLING_EVENT, subscribing_process);
  process_post(&oxygen_saturation_sensor_process, OXYGEN_SATURATION_START_SAMPLING_EVENT, subscribing_process);
}
/*---------------------------------------------------------------------------*/
void
sensors_cmd_stop_sampling(void)
{
  process_post(&heart_rate_sensor_process, HEART_RATE_STOP_SAMPLING_EVENT, NULL);
  process_post(&blood_pressure_sensor_process, BLOOD_PRESSURE_STOP_SAMPLING_EVENT, NULL);
  process_post(&temperature_sensor_process, TEMPERATURE_STOP_SAMPLING_EVENT, NULL);
  process_post(&respiration_sensor_process, RESPIRATION_STOP_SAMPLING_EVENT, NULL);
  process_post(&oxygen_saturation_sensor_process, OXYGEN_SATURATION_STOP_SAMPLING_EVENT, NULL);
}
/*---------------------------------------------------------------------------*/
void
sensors_cmd_stop_processes(void)
{
  process_exit(&heart_rate_sensor_process);
  process_exit(&blood_pressure_sensor_process);
  process_exit(&temperature_sensor_process);
  process_exit(&respiration_sensor_process);
  process_exit(&oxygen_saturation_sensor_process);
}
/*---------------------------------------------------------------------------*/
/** @} */
