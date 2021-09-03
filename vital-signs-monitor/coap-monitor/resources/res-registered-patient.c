/**
 * \file
 *         Implementation of the registered patient resource
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup res-registered-patient
 * @{
 */

#include "contiki.h"
#include "os/sys/log.h"
#include "os/net/app-layer/coap/coap-engine.h"
#include "../../common/json-message.h"
#include "../utils/coap-monitor-constants.h"
#include "./res-registered-patient.h"

#define LOG_MODULE "Resource " COAP_MONITOR_REGISTERED_PATIENT_RESOURCE
#define LOG_LEVEL LOG_LEVEL_COAP_RESOURCES

static void event_handler(void);
static void get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer,
                        uint16_t preferred_size, int32_t *offset);

/* Resource value. */
static char registeredPatient[COAP_MONITOR_PATIENT_ID_LENGTH];

EVENT_RESOURCE(res_registered_patient,
               "title =\"Registered patient\";obs",
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
  json_message_patient_registration(message, COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE, NULL, registeredPatient);
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
  coap_notify_observers(&res_registered_patient);
}
/*---------------------------------------------------------------------------*/
void
res_registered_patient_activate(void)
{
  LOG_DBG("Activating the resource.\n");
  coap_activate_resource(&res_registered_patient, COAP_MONITOR_REGISTERED_PATIENT_RESOURCE);
}
/*---------------------------------------------------------------------------*/
void
res_registered_patient_update
(char *patient_id)
{
  LOG_DBG("Updating the resource value.\n");
  memset(registeredPatient, 0, COAP_MONITOR_PATIENT_ID_LENGTH);
  memcpy(registeredPatient, patient_id, COAP_MONITOR_PATIENT_ID_LENGTH);
  registeredPatient[COAP_MONITOR_PATIENT_ID_LENGTH - 1] = '\0';
  res_registered_patient.trigger();
}
/*---------------------------------------------------------------------------*/
/** @} */
