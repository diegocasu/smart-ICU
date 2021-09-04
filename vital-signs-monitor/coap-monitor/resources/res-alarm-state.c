/**
 * \file
 *         Implementation of the alarm state resource
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup res-alarm-state
 * @{
 */

#include "contiki.h"
#include "os/sys/log.h"
#include "os/net/app-layer/coap/coap-engine.h"
#include "../../common/json-message.h"
#include "../utils/coap-monitor-constants.h"
#include "./res-alarm-state.h"

#define LOG_MODULE "Resource " COAP_MONITOR_ALARM_STATE_RESOURCE
#define LOG_LEVEL LOG_LEVEL_COAP_RESOURCES

static void event_handler(void);
static void get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                        uint16_t preferred_size, int32_t *offset);
static void put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                        uint16_t preferred_size, int32_t *offset);

/* Resource value. */
static alarm_state current_alarm_state;
static struct alarm_system *alarm_system;

EVENT_RESOURCE(res_alarm_state,
               "title =\"Alarm state\";obs",
               get_handler,
               NULL,
               put_handler,
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

  if(current_alarm_state == ALARM_ON) {
    json_message_alarm_started(message, COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE);
  } else {
    json_message_alarm_stopped(message, COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE);
  }

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
put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
            uint16_t preferred_size, int32_t *offset)
{
  const uint8_t *request_payload = NULL;
  char turn_on_alarm_msg[COAP_MONITOR_INPUT_BUFFER_SIZE];
  char turn_off_alarm_msg[COAP_MONITOR_INPUT_BUFFER_SIZE];

  LOG_DBG("Handling a PUT request.\n");
  json_message_alarm_started(turn_on_alarm_msg, COAP_MONITOR_INPUT_BUFFER_SIZE);
  json_message_alarm_started(turn_off_alarm_msg, COAP_MONITOR_INPUT_BUFFER_SIZE);
  coap_get_payload(request, &request_payload);

  if(strcmp(turn_on_alarm_msg, (const char*)request_payload) == 0) {
    LOG_DBG("PUT request for turning on the alarm.\n");
    alarm_start(alarm_system);
    current_alarm_state = ALARM_ON;
    coap_set_status_code(response, CREATED_2_01);
    return;
  }

  if(strcmp(turn_off_alarm_msg, (const char*)request_payload) == 0) {
    LOG_DBG("Turning off the alarm via PUT requests is currently not supported.\n");
    coap_set_status_code(response, NOT_IMPLEMENTED_5_01);
    return;
  }

  LOG_DBG("Unrecognized format of the PUT request.\n");
  coap_set_status_code(response, BAD_REQUEST_4_00);
}
/*---------------------------------------------------------------------------*/
static void
event_handler(void)
{
  LOG_DBG("Notifying the observers.\n");
  coap_notify_observers(&res_alarm_state);
}
/*---------------------------------------------------------------------------*/
void
res_alarm_state_activate(struct alarm_system *alarm)
{
  LOG_DBG("Activating the resource.\n");
  alarm_system = alarm;
  current_alarm_state = alarm_system->state;
  coap_activate_resource(&res_alarm_state, COAP_MONITOR_ALARM_STATE_RESOURCE);
}
/*---------------------------------------------------------------------------*/
void
res_alarm_state_update
(alarm_state alarm_state)
{
  LOG_DBG("Updating the resource value.\n");
  current_alarm_state = alarm_state;
  res_alarm_state.trigger();
}
/*---------------------------------------------------------------------------*/
/** @} */
