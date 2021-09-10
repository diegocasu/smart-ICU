/**
 * \file
 *         Implementation of the oxygen saturation resource
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup res-oxygen-saturation
 * @{
 */

#include "contiki.h"
#include "os/sys/log.h"
#include "os/net/app-layer/coap/coap-engine.h"
#include "../../common/json-message.h"
#include "../../common/sensors/utils/sensor-constants.h"
#include "../utils/coap-monitor-constants.h"
#include "./res-oxygen-saturation.h"

#define LOG_MODULE "Resource " COAP_MONITOR_OXYGEN_SATURATION_RESOURCE
#define LOG_LEVEL LOG_LEVEL_COAP_RESOURCES

static void event_handler(void);
static void get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                        uint16_t preferred_size, int32_t *offset);

/* Resource value. */
static int oxygen_saturation_sample;

EVENT_RESOURCE(res_oxygen_saturation,
               "title =\"Oxygen saturation\";obs",
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
  json_message_oxygen_saturation_sample(message, COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE, oxygen_saturation_sample);
  length = sizeof(message) - 1;

  /* Send the response. */
  memcpy(buffer, message, length);
  coap_set_header_content_format(response, APPLICATION_JSON);
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_option(response, COAP_OPTION_MAX_AGE);
  coap_set_header_max_age(response, OXYGEN_SATURATION_SAMPLING_INTERVAL);
  coap_set_payload(response, buffer, length);
  coap_set_status_code(response, CONTENT_2_05);
}
/*---------------------------------------------------------------------------*/
static void
event_handler(void)
{
  LOG_DBG("Notifying the observers.\n");
  coap_notify_observers(&res_oxygen_saturation);
}
/*---------------------------------------------------------------------------*/
void
res_oxygen_saturation_activate(void)
{
  LOG_DBG("Activating the resource.\n");
  oxygen_saturation_sample = -1;
  coap_activate_resource(&res_oxygen_saturation, COAP_MONITOR_OXYGEN_SATURATION_RESOURCE);
}
/*---------------------------------------------------------------------------*/
void
res_oxygen_saturation_update
(int oxygen_saturation)
{
  LOG_DBG("Updating the resource value.\n");
  oxygen_saturation_sample = oxygen_saturation;
  res_oxygen_saturation.trigger();
}
/*---------------------------------------------------------------------------*/
/** @} */
