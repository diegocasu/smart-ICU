/**
 * \file
 *         Implementation of the functions generating JSON payloads for MQTT/CoAP messages.
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup json-message
 * @{
 */

#include <stdio.h>
#include <string.h>
#include "os/sys/clock.h"
#include "json-message.h"
#include "../sensors/utils/sensor-constants.h"

/*---------------------------------------------------------------------------*/
static void
clear_buffer(char *buffer, size_t size)
{
  memset(buffer, 0, size);
}
/*---------------------------------------------------------------------------*/
void
json_message_device_registration(char *message_buffer, size_t size, char *monitor_id)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer, size, "{\"monitorID\": \"%s\", \"registration\": true}", monitor_id);
}
/*---------------------------------------------------------------------------*/
void
json_message_patient_registration(char *message_buffer, size_t size, char *monitor_id, char *patient_id)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer, size, "{\"monitorID\": \"%s\", \"patientID\": \"%s\"}", monitor_id, patient_id);
}
/*---------------------------------------------------------------------------*/
void
json_message_alarm_started(char *message_buffer, size_t size)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer, size, "%s", "{\"alarm\": true}");
}
/*---------------------------------------------------------------------------*/
void
json_message_alarm_stopped(char *message_buffer, size_t size)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer, size, "%s", "{\"alarm\": false}");
}
/*---------------------------------------------------------------------------*/
void
json_message_heart_rate_sample(char *message_buffer, size_t size, int sample)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer,
           size,
           "{\"heartRate\": %d, \"unit\": \"%s\", \"timestamp\": %ld}",
           sample,
           HEART_RATE_UNIT,
           clock_seconds());
}
/*---------------------------------------------------------------------------*/
void json_message_blood_pressure_sample(char *message_buffer, size_t size, int sample)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer,
           size,
           "{\"bloodPressure\": %d, \"unit\": \"%s\", \"timestamp\": %ld}",
           sample,
           BLOOD_PRESSURE_UNIT,
           clock_seconds());
}
/*---------------------------------------------------------------------------*/
void json_message_oxygen_saturation_sample(char *message_buffer, size_t size, int sample)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer,
           size,
           "{\"oxygenSaturation\": %d, \"unit\": \"%s\", \"timestamp\": %ld}",
           sample,
           OXYGEN_SATURATION_UNIT,
           clock_seconds());
}
/*---------------------------------------------------------------------------*/
void json_message_respiration_sample(char *message_buffer, size_t size, int sample)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer,
           size,
           "{\"respiration\": %d, \"unit\": \"%s\", \"timestamp\": %ld}",
           sample,
           RESPIRATION_UNIT,
           clock_seconds());
}
/*---------------------------------------------------------------------------*/
void json_message_temperature_sample(char *message_buffer, size_t size, int sample)
{
  clear_buffer(message_buffer, size);
  snprintf(message_buffer,
           size,
           "{\"temperature\": %d, \"unit\": \"%s\", \"timestamp\": %ld}",
           sample,
           TEMPERATURE_UNIT,
           clock_seconds());
}
/*---------------------------------------------------------------------------*/
/** @} */
