/**
 * \file
 *         Implementation of the heart rate resource
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup res-heart-rate
 * @{
 */

#include "contiki.h"
#include "os/sys/log.h"
#include "os/net/app-layer/coap/coap-engine.h"
#include "../../common/json-message.h"
#include "../utils/coap-monitor-constants.h"
#include "./res-heart-rate.h"

#define LOG_MODULE "Resource " COAP_MONITOR_HEART_RATE_RESOURCE
#define LOG_LEVEL LOG_LEVEL_COAP_RESOURCES

static void event_handler(void);
static void get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                        uint16_t preferred_size, int32_t *offset);

/* Resource value. */
static int heart_rate_sample;

EVENT_RESOURCE(res_heart_rate,
               "title =\"Heart rate\";obs",
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
  json_message_heart_rate_sample(message, COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE, heart_rate_sample);
  length = sizeof(message) - 1;

  /* Send the response. */
  memcpy(buffer, message, length);
  coap_set_header_content_format(response, APPLICATION_JSON);
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);
  coap_set_status_code(response, CONTENT_2_05);
}
/*---------------------------------------------------------------------------*/
static void
event_handler(void)
{
  LOG_DBG("Notifying the observers.\n");
  coap_notify_observers(&res_heart_rate);
}
/*---------------------------------------------------------------------------*/
void
res_heart_rate_activate(void)
{
  LOG_DBG("Activating the resource.\n");
  heart_rate_sample = -1;
  coap_activate_resource(&res_heart_rate, COAP_MONITOR_HEART_RATE_RESOURCE);
}
/*---------------------------------------------------------------------------*/
void
res_heart_rate_update
(int heart_rate)
{
  LOG_DBG("Updating the resource value.\n");
  heart_rate_sample = heart_rate;
  res_heart_rate.trigger();
}
/*---------------------------------------------------------------------------*/
/** @} */
