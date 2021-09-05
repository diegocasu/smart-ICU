/**
 * \file
 *         Implementation of the blood pressure resource
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup res-blood-pressure
 * @{
 */

#include "contiki.h"
#include "os/sys/log.h"
#include "os/net/app-layer/coap/coap-engine.h"
#include "../../common/json-message.h"
#include "../../sensors/utils/sensor-constants.h"
#include "../utils/coap-monitor-constants.h"
#include "./res-blood-pressure.h"

#define LOG_MODULE "Resource " COAP_MONITOR_BLOOD_PRESSURE_RESOURCE
#define LOG_LEVEL LOG_LEVEL_COAP_RESOURCES

static void event_handler(void);
static void get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                        uint16_t preferred_size, int32_t *offset);

/* Resource value. */
static int blood_pressure_sample;

EVENT_RESOURCE(res_blood_pressure,
               "title =\"Blood pressure\";obs",
               get_handler,
               NULL,
               NULL,
               NULL,
               event_handler);

/*---------------------------------------------------------------------------*/
static void
get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
            uint16_t preferred_size, int32_t *offset)
{
  char message[COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE];
  int length;

  /* Prepare the message. */
  LOG_DBG("Handling a GET request.\n");
  json_message_blood_pressure_sample(message, COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE, blood_pressure_sample);
  length = sizeof(message) - 1;

  /* Send the response. */
  memcpy(buffer, message, length);
  coap_set_header_content_format(response, APPLICATION_JSON);
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_option(response, COAP_OPTION_MAX_AGE);
  coap_set_header_max_age(response, BLOOD_PRESSURE_SAMPLING_INTERVAL);
  coap_set_payload(response, buffer, length);
  coap_set_status_code(response, CONTENT_2_05);
}
/*---------------------------------------------------------------------------*/
static void
event_handler(void)
{
  LOG_DBG("Notifying the observers.\n");
  coap_notify_observers(&res_blood_pressure);
}
/*---------------------------------------------------------------------------*/
void
res_blood_pressure_activate(void)
{
  LOG_DBG("Activating the resource.\n");
  blood_pressure_sample = -1;
  coap_activate_resource(&res_blood_pressure, COAP_MONITOR_BLOOD_PRESSURE_RESOURCE);
}
/*---------------------------------------------------------------------------*/
void
res_blood_pressure_update
(int blood_pressure)
{
  LOG_DBG("Updating the resource value.\n");
  blood_pressure_sample = blood_pressure;
  res_blood_pressure.trigger();
}
/*---------------------------------------------------------------------------*/
/** @} */
