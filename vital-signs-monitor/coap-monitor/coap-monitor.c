/**
 * \file
 *         Implementation of a CoAP vital signs monitor
 * \author
 *         Diego Casu
 */

/**
 * \defgroup coap-monitor CoAP vital signs monitor
 * @{
 *
 * The coap-monitor module provides a simulation of a vital signs monitor
 * that uses the CoAP protocol to exchange data with a collector.
 */

#include <stdbool.h>
#include "contiki.h"
#include "os/sys/log.h"
#include "os/sys/node-id.h"
#include "os/sys/etimer.h"
#include "os/net/ipv6/uiplib.h"
#include "os/net/ipv6/uip-ds6.h"
#include "os/net/app-layer/coap/coap-engine.h"
#include "os/net/app-layer/coap/coap-blocking-api.h"
#include "os/dev/serial-line.h"
#include "os/dev/button-hal.h"
#include "../common/sensors-cmd.h"
#include "../common/json-message.h"
#include "../common/alarm.h"
#include "../common/alarm-constants.h"
#include "./utils/coap-monitor-constants.h"
#include "./resources/res-registered-patient.h"
#include "./resources/res-heart-rate.h"
#include "./resources/res-blood-pressure.h"
#include "./resources/res-temperature.h"
#include "./resources/res-respiration.h"
#include "./resources/res-oxygen-saturation.h"
#include "./resources/res-alarm-state.h"

#define LOG_MODULE "CoAP vital signs monitor"
#define LOG_LEVEL LOG_LEVEL_COAP_MONITOR

PROCESS_NAME(coap_vital_signs_monitor);

/* Structure representing a CoAP vital signs monitor. */
struct coap_monitor {
  char monitor_id[COAP_MONITOR_ID_LENGTH];
  struct alarm_system alarm;
  uint8_t state;

  /* ID of the patient currently attached to the monitor. */
  char patient_id[COAP_MONITOR_PATIENT_ID_LENGTH];

  /* Timer to check network connectivity. */
  clock_time_t network_check_interval;
  struct etimer network_check_timer;

  /* Management of monitor registration messages. */
  coap_endpoint_t collector_endpoint;
  coap_message_t registration_request;
  char output_buffer[COAP_MONITOR_OUTPUT_BUFFER_SIZE];
};

static struct coap_monitor monitor;

/*---------------------------------------------------------------------------*/
/**
 * \brief                 Check if a sample should trigger an alarm.
 * \param min_threshold   The minimum threshold that should trigger an alarm.
 * \param max_threshold   The maximum threshold that should trigger an alarm.
 * \param sample          The sample to be checked.
 * \return                true if the event should trigger an alarm, false otherwise.
 *
 *                        The function checks if a sample should trigger an alarm,
 *                        i.e. if the sample is less than or equal to <code>min_threshold</code>,
 *                        or greater than or equal to <code>max_threshold</code>.
 */
static bool
alarming_sample(int min_threshold, int max_threshold, int sample)
{
  if(sample <= min_threshold || sample >= max_threshold) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief    Check if the monitor is correctly connected to the network.
 * \return   true if the monitor is correctly connected to the network,
 *           false otherwise.
 *
 *           The function checks if the monitor is correctly connected
 *           to the network, namely if it has a global address
 *           and a default route.
 */
static bool
network_ready(void)
{
  if(uip_ds6_get_global(ADDR_PREFERRED) == NULL || uip_ds6_defrt_choose() == NULL) {
    return false;
  }
  return true;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the COAP_MONITOR_STATE_STARTED state.
 *
 *          The function handles the COAP_MONITOR_STATE_STARTED state,
 *          checking if the monitor is correctly connected to the network.
 *          If the latter is true, it changes the monitor state
 *          to COAP_MONITOR_STATE_NETWORK_READY and stops the
 *          <code>network_check_timer</code>.
 */
static void
handle_state_started(void)
{
  if(network_ready()) {
    LOG_INFO("Connected to the network. ");
    LOG_INFO_("Global address: ");
    LOG_INFO_6ADDR(&(uip_ds6_get_global(ADDR_PREFERRED)->ipaddr));
    LOG_INFO_(". Link local address: ");
    LOG_INFO_6ADDR(&(uip_ds6_get_link_local(ADDR_PREFERRED)->ipaddr));
    LOG_INFO_("\n");
    monitor.state = COAP_MONITOR_STATE_NETWORK_READY;
  } else {
    LOG_INFO("Connecting to the network.\n");
    etimer_reset(&monitor.network_check_timer);
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief    Handle the COAP_MONITOR_STATE_NETWORK_READY state.
 *
 *           The function handles the COAP_MONITOR_STATE_NETWORK_READY state,
 *           initializing the monitor ID and preparing the monitor registration
 *           message to send to the collector. The latter is placed in
 *           <code>monitor.output_buffer</code>, ready to be sent via
 *           <code>COAP_BLOCKING_REQUEST()</code>.
 */
static void
handle_state_network_ready()
{
  /* Initialize the monitor ID as the global IPv6 address. */
  uiplib_ipaddr_snprint(monitor.monitor_id,
                        COAP_MONITOR_ID_LENGTH,
                        &(uip_ds6_get_global(ADDR_PREFERRED)->ipaddr));

  /* Prepare the registration request to send to the collector. */
  coap_init_message(&monitor.registration_request, COAP_TYPE_CON, COAP_POST, 0);
  coap_set_header_uri_path(&monitor.registration_request, COAP_MONITOR_COLLECTOR_REGISTERED_MONITORS_RESOURCE);

  json_message_monitor_registration(monitor.output_buffer, COAP_MONITOR_OUTPUT_BUFFER_SIZE, monitor.monitor_id);
  coap_set_payload(&monitor.registration_request, monitor.output_buffer, sizeof(monitor.output_buffer) - 1);

  LOG_INFO("Sending a POST with payload %s to the endpoint %s/%s.\n",
           monitor.output_buffer,
           COAP_MONITOR_COLLECTOR_REGISTERED_MONITORS_RESOURCE,
           COAP_MONITOR_COLLECTOR_ENDPOINT);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the reception of a new patient ID on the serial line.
 *
 *          The function handles the reception of a new patient ID
 *          on the serial line, updating the relative resource
 *          and restarting the sampling activity
 *          of the sensor processes. It changes the
 *          monitor state to COAP_MONITOR_STATE_OPERATIONAL.
 */
static void
handle_new_patient_ID(char *patient_id)
{
  memcpy(monitor.patient_id, patient_id, COAP_MONITOR_PATIENT_ID_LENGTH);
  monitor.patient_id[COAP_MONITOR_PATIENT_ID_LENGTH - 1] = '\0';
  LOG_INFO("New patient ID: %s.\n", monitor.patient_id);

  /* Update the patient ID resource. */
  res_registered_patient_update(monitor.patient_id);

  /* Start the sampling activity of the sensors. */
  sensors_cmd_start_sampling(&coap_vital_signs_monitor);

  monitor.state = COAP_MONITOR_STATE_OPERATIONAL;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the CoAP message sent by the collector in response to
 *          the monitor registration message.
 *
 *          The function handles the CoAP message sent by the collector
 *          in response to the monitor registration message. More in details:<br>
 *          1) if the request fails due to a timeout or the response carries a
 *             response code different from 2.01, the function changes the monitor
 *             state to COAP_MONITOR_STATE_REGISTRATION_FAILED;<br>
 *          2) if the request succeeds with a response code 2.01, the function changes
 *             the monitor state to COAP_MONITOR_STATE_WAITING_PATIENT_ID and starts
 *             the sampling activity of the sensors.
 */
void
handle_registration_response(coap_message_t *response)
{
  if(response == NULL) {
    LOG_ERR("Registration failed: request timed out.\n");
    monitor.state = COAP_MONITOR_STATE_REGISTRATION_FAILED;
    return;
  }

  if(response->code != CREATED_2_01) {
    LOG_ERR("Registration failed. Response code: %u.%u\n", (response->code) >> 5, (response->code) & 31);
    monitor.state = COAP_MONITOR_STATE_REGISTRATION_FAILED;
    return;
  }

  LOG_INFO("Registration succeeded.\n");

  /* Start the sensor processes (without starting the sampling activity). */
  sensors_cmd_start_processes();

  monitor.state = COAP_MONITOR_STATE_WAITING_PATIENT_ID;
  LOG_INFO("Waiting for a new patient ID on the serial line.\n");

#ifdef AUTOMATIC_PATIENT_ID_CONFIGURATION
  LOG_INFO("Automatic configuration of the new patient ID.\n");
  char random_patient_ID[COAP_MONITOR_PATIENT_ID_LENGTH];
  snprintf(random_patient_ID, COAP_MONITOR_PATIENT_ID_LENGTH, "auto_%d", rand());
  handle_new_patient_ID(random_patient_ID);
#endif
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the button press event.
 *
 *          The function handles the button press event, turning off
 *          the alarm system and resetting the patient ID if the press
 *          duration exceeds a configured number of seconds.
 *          If the patient ID is reset, it changes the
 *          monitor state to COAP_MONITOR_STATE_WAITING_PATIENT_ID.
 */
static void
handle_button_press(button_hal_button_t *button)
{
  LOG_INFO("Button press event: %d s.\n", button->press_duration_seconds);

  if(button->press_duration_seconds == COAP_MONITOR_RESET_ALARM_DURATION
     || button->press_duration_seconds == COAP_MONITOR_RESET_PATIENT_ID_DURATION) {
    bool alarm_state_changed;

    /* The alarm is stopped and the relative resource is updated, if the alarm was turned on. */
    LOG_INFO("Resetting the alarm.\n");
    alarm_state_changed = alarm_stop(&monitor.alarm);

    if(alarm_state_changed) {
      res_alarm_state_update(ALARM_OFF);
    }
  }

  if(button->press_duration_seconds == COAP_MONITOR_RESET_PATIENT_ID_DURATION) {
    LOG_INFO("Resetting the patient ID.\n");
    memset(monitor.patient_id, 0, COAP_MONITOR_PATIENT_ID_LENGTH);

    /* Update the patient ID resource. */
    res_registered_patient_update(monitor.patient_id);

    /* Stop the sampling activity of the sensors. */
    sensors_cmd_stop_sampling();

    monitor.state = COAP_MONITOR_STATE_WAITING_PATIENT_ID;
    LOG_INFO("Waiting for a new patient ID on the serial line.\n");

#ifdef AUTOMATIC_PATIENT_ID_CONFIGURATION
    LOG_INFO("Automatic configuration of the new patient ID.\n");
    char random_patient_ID[COAP_MONITOR_PATIENT_ID_LENGTH];
    snprintf(random_patient_ID, COAP_MONITOR_PATIENT_ID_LENGTH, "auto_%d", rand());
    handle_new_patient_ID(random_patient_ID);
#endif
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the reception of a sample from a sensor process.
 *
 *          The function handles the reception of a sample from a
 *          sensor process, updating the corresponding resource.
 *          If the sample is an alarming one, it turns
 *          on the alarm system and updates the relative resource.
 */
static void
handle_sensor_sample(process_event_t event, int sample)
{
  char *sensor;
  int min_threshold;
  int max_threshold;

  if(sensors_cmd_heart_rate_sample_event(event)) {
    sensor = "heart rate";
    min_threshold = ALARM_HEART_RATE_MIN_THRESHOLD;
    max_threshold = ALARM_HEART_RATE_MAX_THRESHOLD;
    res_heart_rate_update(sample);

  } else if(sensors_cmd_blood_pressure_sample_event(event)) {
    sensor = "blood pressure";
    min_threshold = ALARM_BLOOD_PRESSURE_MIN_THRESHOLD;
    max_threshold = ALARM_BLOOD_PRESSURE_MAX_THRESHOLD;
    res_blood_pressure_update(sample);

  } else if(sensors_cmd_oxygen_saturation_sample_event(event)) {
    sensor = "oxygen saturation";
    min_threshold = ALARM_OXYGEN_SATURATION_MIN_THRESHOLD;
    max_threshold = ALARM_OXYGEN_SATURATION_MAX_THRESHOLD;
    res_oxygen_saturation_update(sample);

  } else if(sensors_cmd_respiration_sample_event(event)) {
    sensor = "respiration";
    min_threshold = ALARM_RESPIRATION_MIN_THRESHOLD;
    max_threshold = ALARM_RESPIRATION_MAX_THRESHOLD;
    res_respiration_update(sample);

  } else if(sensors_cmd_temperature_sample_event(event)) {
    sensor = "temperature";
    min_threshold = ALARM_TEMPERATURE_MIN_THRESHOLD;
    max_threshold = ALARM_TEMPERATURE_MAX_THRESHOLD;
    res_temperature_update(sample);

  } else {
    LOG_ERR("Dropping a sample from an unhandled sensor process.\n");
    return;
  }

  if(alarming_sample(min_threshold, max_threshold, sample)) {
    bool alarm_state_changed;

    LOG_INFO("Alarming %s sample detected: %d. Min threshold: %d, max threshold: %d\n",
             sensor, sample, min_threshold, max_threshold);
    LOG_INFO("Starting the alarm.\n");

    alarm_state_changed = alarm_start(&monitor.alarm);
    if(alarm_state_changed) {
      res_alarm_state_update(ALARM_ON);
    }
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Initialize the state, timer and resources of the monitor.
 */
static void
init_monitor()
{
  monitor.state = COAP_MONITOR_STATE_STARTED;

  /* Initialize the alarm system. */
  alarm_init(&monitor.alarm);

  /* Initialize the collector endpoint. */
  coap_endpoint_parse(COAP_MONITOR_COLLECTOR_ENDPOINT,
                      strlen(COAP_MONITOR_COLLECTOR_ENDPOINT),
                      &monitor.collector_endpoint);

  /* Activate the resources. */
  res_registered_patient_activate();
  res_alarm_state_activate(&monitor.alarm);
  res_heart_rate_activate();
  res_blood_pressure_activate();
  res_temperature_activate();
  res_respiration_activate();
  res_oxygen_saturation_activate();

  /* Initialize the periodic timer to check the network connectivity. */
  monitor.network_check_interval = COAP_MONITOR_NETWORK_CHECK_INTERVAL*CLOCK_SECOND;
  etimer_set(&monitor.network_check_timer, monitor.network_check_interval);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Terminate the monitor stopping the processes simulating
 *          the sensors and the alarm system.
 */
static void
finish_monitor()
{
  sensors_cmd_stop_sampling();
  sensors_cmd_stop_processes();
  alarm_stop(&monitor.alarm);
}
/*---------------------------------------------------------------------------*/
/*
 * Process simulating a vital signs monitor using the CoAP protocol
 * to exchange data with a collector.
 * The process activates a set of CoAP observable resources, so that:
 * 1) telemetry data coming from a set of simulated sensor processes
 *    can be sent to the collector;
 * 2) commands able to trigger the alarm system can be received.
 * The monitor requires a patient ID in order to be fully operational, which can
 * be passed via the serial line at startup. The patient ID can be reset and
 * re-inserted pressing the button of the monitor for at least 10 seconds.
 * If the alarm system is triggered, the alarm state can be turned off by
 * pressing the same button for at least 5 seconds.
 */
PROCESS(coap_vital_signs_monitor, "CoAP vital signs monitor");
AUTOSTART_PROCESSES(&coap_vital_signs_monitor);

/* The event must be named exactly "ev", otherwise COAP_BLOCKING_REQUEST() does not work. */
PROCESS_THREAD(coap_vital_signs_monitor, ev, data)
{
  PROCESS_BEGIN();
  LOG_INFO("Process started.\n");
  init_monitor();

  while(true) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER) {
      if(monitor.state == COAP_MONITOR_STATE_STARTED) {
        handle_state_started();
      }

      if(monitor.state == COAP_MONITOR_STATE_NETWORK_READY) {
        handle_state_network_ready();

        /*
         * The registration message is placed in the output buffer by handle_state_network_ready().
         * This is done because COAP_BLOCKING_REQUEST() must be in the main process body to work correctly.
         */
        COAP_BLOCKING_REQUEST(&monitor.collector_endpoint,&monitor.registration_request, handle_registration_response);
      }

      /* COAP_MONITOR_STATE_REGISTRATION_FAILED is set by handle_registration_response(). */
      if(monitor.state == COAP_MONITOR_STATE_REGISTRATION_FAILED) {
        break;
      }
    }

    if(ev == serial_line_event_message && monitor.state == COAP_MONITOR_STATE_WAITING_PATIENT_ID) {
      handle_new_patient_ID((char*)data);
      continue;
    }

    if(ev == button_hal_periodic_event && monitor.state == COAP_MONITOR_STATE_OPERATIONAL) {
      handle_button_press((button_hal_button_t *)data);
      continue;
    }

    if(sensors_cmd_sample_event(ev) && monitor.state == COAP_MONITOR_STATE_OPERATIONAL) {
      handle_sensor_sample(ev, *((int *)data));
      continue;
    }
  }

  finish_monitor();
  LOG_INFO("Stopping the process.\n");
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
