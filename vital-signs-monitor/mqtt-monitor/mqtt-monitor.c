/**
 * \file
 *         Implementation of an MQTT vital signs monitor
 * \author
 *         Diego Casu
 */

/**
 * \defgroup mqtt-monitor MQTT vital signs monitor
 * @{
 *
 * The mqtt-output-queue module provides a simulation of a vital signs monitor
 * that uses the MQTT protocol to exchange data with a collector.
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "contiki.h"
#include "os/sys/log.h"
#include "os/sys/node-id.h"
#include "os/net/ipv6/uiplib.h"
#include "os/net/app-layer/mqtt/mqtt.h"
#include "os/dev/serial-line.h"
#include "os/dev/button-hal.h"
#include "../common/json-message.h"
#include "../common/alarm.h"
#include "../common/alarm-constants.h"
#include "../sensors/utils/prng.h"
#include "../sensors/heart-rate.h"
#include "../sensors/blood-pressure.h"
#include "../sensors/temperature.h"
#include "../sensors/respiration.h"
#include "../sensors/oxygen-saturation.h"
#include "./utils/mqtt-output-queue.h"
#include "./utils/mqtt-monitor-constants.h"

#define LOG_MODULE "MQTT vital signs monitor"
#define LOG_LEVEL LOG_LEVEL_MQTT_MONITOR

PROCESS_NAME(mqtt_vital_signs_monitor);

/* Structure representing an MQTT vital signs monitor. */
struct mqtt_monitor {
  char monitor_id[MQTT_MONITOR_ID_LENGTH];
  struct alarm_system alarm;

  /* Internal state. */
  clock_time_t state_check_interval;
  struct etimer state_check_timer;
  uint8_t state;

  /* ID of the patient currently attached to the monitor. */
  char patient_id[MQTT_MONITOR_PATIENT_ID_LENGTH];

  /*
   * Management of the MQTT connection and of the MQTT message output queue.
   * The latter is not implemented by default in the Contiki module
   * (the Contiki MQTT "output queue" has only space for one message at a time).
   */
  struct mqtt_module {
    struct mqtt_connection connection;
    mqtt_status_t status;
    struct mqtt_output_queue output_queue;
    struct ctimer output_queue_timer;
    clock_time_t output_queue_timer_interval;
  } mqtt_module;

  /* Buffers used to store the topics regarding commands. */
  struct cmd_topics {
    char patient_registration[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char device_registration[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char alarm_state[MQTT_MONITOR_TOPIC_MAX_LENGTH];
  } cmd_topics;

  /* Buffers used to store the topics regarding telemetry data. */
  struct telemetry_topics {
    char oxygen_saturation[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char blood_pressure[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char temperature[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char respiration[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char alarm_state[MQTT_MONITOR_TOPIC_MAX_LENGTH];
    char heart_rate[MQTT_MONITOR_TOPIC_MAX_LENGTH];
  } telemetry_topics;

  /* Buffers used to store the output messages. */
  struct output_buffers {
    char patient_registration[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char device_registration[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char oxygen_saturation[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char blood_pressure[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char temperature[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char respiration[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char alarm_state[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
    char heart_rate[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
  } output_buffers;
};

static struct mqtt_monitor monitor;

/*---------------------------------------------------------------------------*/
/**
 * \brief         Check if an event is a notification of a new sample
 *                sent by a sensor process.
 * \param event   The event to be checked.
 * \return        true if the event is a notification of a new sample,
 *                false otherwise.
 */
static bool
sensor_sample_event(process_event_t event)
{
  if(event == HEART_RATE_SAMPLE_EVENT
     || event == BLOOD_PRESSURE_SAMPLE_EVENT
     || event == OXYGEN_SATURATION_SAMPLE_EVENT
     || event == RESPIRATION_SAMPLE_EVENT
     || event == TEMPERATURE_SAMPLE_EVENT) {
    return true;
  }
  return false;
}
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
 * \brief    Check if the device is correctly connected to the network.
 * \return   true if the device is correctly connected to the network,
 *           false otherwise.
 *
 *           The function checks if the device is correctly connected
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
 * \brief   Initialize the buffers holding the command and telemetry topics.
 *
 *          The function initializes the buffers holding the
 *          command and telemetry topics. It must be called
 *          after the monitor ID has been initialized,
 *          which is done in <code>network_ready()</code>.
 */
static void
init_topics(void)
{
  /* Command topics. */
  snprintf(monitor.cmd_topics.alarm_state, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_CMD_TOPIC_ALARM_STATE, monitor.monitor_id);
  snprintf(monitor.cmd_topics.device_registration, MQTT_MONITOR_TOPIC_MAX_LENGTH, "%s", MQTT_MONITOR_CMD_TOPIC_DEVICE_REGISTRATION);
  snprintf(monitor.cmd_topics.patient_registration, MQTT_MONITOR_TOPIC_MAX_LENGTH, "%s",MQTT_MONITOR_CMD_TOPIC_PATIENT_REGISTRATION);

  /* Telemetry topics. */
  snprintf(monitor.telemetry_topics.heart_rate, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_TELEMETRY_TOPIC_HEART_RATE, monitor.monitor_id);
  snprintf(monitor.telemetry_topics.blood_pressure, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_TELEMETRY_TOPIC_BLOOD_PRESSURE, monitor.monitor_id);
  snprintf(monitor.telemetry_topics.temperature, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_TELEMETRY_TOPIC_TEMPERATURE, monitor.monitor_id);
  snprintf(monitor.telemetry_topics.respiration, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_TELEMETRY_TOPIC_RESPIRATION, monitor.monitor_id);
  snprintf(monitor.telemetry_topics.oxygen_saturation, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_TELEMETRY_TOPIC_OXYGEN_SATURATION, monitor.monitor_id);
  snprintf(monitor.telemetry_topics.alarm_state, MQTT_MONITOR_TOPIC_MAX_LENGTH, MQTT_MONITOR_TELEMETRY_TOPIC_ALARM_STATE, monitor.monitor_id);

  LOG_DBG("Command alarm state topic: %s\n", monitor.cmd_topics.alarm_state);
  LOG_DBG("Command device registration topic: %s\n", monitor.cmd_topics.device_registration);
  LOG_DBG("Command patient registration topic: %s\n", monitor.cmd_topics.patient_registration);
  LOG_DBG("Telemetry heart rate topic: %s\n", monitor.telemetry_topics.heart_rate);
  LOG_DBG("Telemetry blood pressure topic: %s\n", monitor.telemetry_topics.blood_pressure);
  LOG_DBG("Telemetry temperature topic: %s\n", monitor.telemetry_topics.temperature);
  LOG_DBG("Telemetry respiration topic: %s\n", monitor.telemetry_topics.respiration);
  LOG_DBG("Telemetry oxygen saturation topic: %s\n", monitor.telemetry_topics.oxygen_saturation);
  LOG_DBG("Telemetry alarm state topic: %s\n", monitor.telemetry_topics.alarm_state);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief                 Publish a message to a topic.
 * \param topic           A pointer to the buffer storing the topic.
 * \param output_buffer   A pointer to the buffer storing the message.
 *
 *                        The function publishes a message to a topic. If the operation
 *                        fails due to a MQTT_STATUS_OUT_QUEUE_FULL error, the function
 *                        stores the message, together with its topic, in the monitor
 *                        output queue, so that a retransmission can be attempted later.
 *                        If the monitor output queue is full too, the message is discarded.<br>
 *                        It should be noted that the MQTT module of Contiki does not
 *                        provide an output queue, so only one message at a time could be
 *                        sent using it as it is.
 */
static void
publish(char *topic, char* output_buffer)
{
  LOG_INFO("Publishing %s in the topic %s.\n", output_buffer, topic);
  monitor.mqtt_module.status = mqtt_publish(&monitor.mqtt_module.connection,
                                            NULL,
                                            topic,
                                            (uint8_t *)output_buffer,
                                            strlen(output_buffer),
                                            MQTT_QOS_LEVEL_0,
                                            MQTT_RETAIN_OFF);
  switch(monitor.mqtt_module.status) {
  case MQTT_STATUS_OK:
    return;
  case MQTT_STATUS_NOT_CONNECTED_ERROR: {
    LOG_ERR("Publishing failed. Error: MQTT_STATUS_NOT_CONNECTED_ERROR.\n");
    return;
  }
  case MQTT_STATUS_OUT_QUEUE_FULL: {
    LOG_ERR("Publishing failed. Error: MQTT_STATUS_OUT_QUEUE_FULL.\n");
    break;
  }
  default:
    LOG_ERR("Publishing failed. Error: unknown.\n");
    return;
  }

  if(!mqtt_output_queue_is_full(&monitor.mqtt_module.output_queue)) {
    LOG_INFO("Enqueuing the message in the output queue.\n");
    mqtt_output_queue_insert(&monitor.mqtt_module.output_queue, output_buffer, topic);
  } else {
    LOG_INFO("The output queue is full. Discarding the message.\n");
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Start the processes simulating the sensors.
 */
static void
start_sensor_processes(void)
{
  prng_init(node_id);
  process_start(&heart_rate_sensor_process, NULL);
  process_start(&blood_pressure_sensor_process, NULL);
  process_start(&temperature_sensor_process, NULL);
  process_start(&respiration_sensor_process, NULL);
  process_start(&oxygen_saturation_sensor_process, NULL);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Start the sampling activity of the processes simulating the sensors.
 */
static void
start_sensor_sampling(void)
{
  process_post(&heart_rate_sensor_process, HEART_RATE_START_SAMPLING_EVENT, &mqtt_vital_signs_monitor);
  process_post(&blood_pressure_sensor_process, BLOOD_PRESSURE_START_SAMPLING_EVENT, &mqtt_vital_signs_monitor);
  process_post(&temperature_sensor_process, TEMPERATURE_START_SAMPLING_EVENT, &mqtt_vital_signs_monitor);
  process_post(&respiration_sensor_process, RESPIRATION_START_SAMPLING_EVENT, &mqtt_vital_signs_monitor);
  process_post(&oxygen_saturation_sensor_process, OXYGEN_SATURATION_START_SAMPLING_EVENT, &mqtt_vital_signs_monitor);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Stop the sampling activity of the processes simulating the sensors.
 */
static void
stop_sensor_sampling(void)
{
  process_post(&heart_rate_sensor_process, HEART_RATE_STOP_SAMPLING_EVENT, NULL);
  process_post(&blood_pressure_sensor_process, BLOOD_PRESSURE_STOP_SAMPLING_EVENT, NULL);
  process_post(&temperature_sensor_process, TEMPERATURE_STOP_SAMPLING_EVENT, NULL);
  process_post(&respiration_sensor_process, RESPIRATION_STOP_SAMPLING_EVENT, NULL);
  process_post(&oxygen_saturation_sensor_process, OXYGEN_SATURATION_STOP_SAMPLING_EVENT, NULL);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Stop the processes simulating the sensors.
 */
static void
stop_sensor_processes(void)
{
  process_exit(&heart_rate_sensor_process);
  process_exit(&blood_pressure_sensor_process);
  process_exit(&temperature_sensor_process);
  process_exit(&respiration_sensor_process);
  process_exit(&oxygen_saturation_sensor_process);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Transmit the first message in the MQTT message output queue.
 *
 *          The function transmits the first message in the MQTT message output queue,
 *          if the latter is not empty. The transmission is done combining an
 *          <code>mqtt_output_queue_extract()</code> and a <code>publish()</code>:
 *          this means that, if a MQTT_STATUS_OUT_QUEUE_FULL error occurs also for the
 *          retransmission, the message is enqueued again (not preserving
 *          its original placement).<br>
 *          This function is the callback of the ctimer initialized for the output queue.
 */
static void
retry_message_transmission()
{
  static char msg[MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
  static char topic[MQTT_MONITOR_TOPIC_MAX_LENGTH];

  LOG_DBG("Output queue size: %d, insert_index: %d, extract_index:%d\n",
           monitor.mqtt_module.output_queue.length, monitor.mqtt_module.output_queue.insert_index,
           monitor.mqtt_module.output_queue.extract_index);

  if(!mqtt_output_queue_is_empty(&monitor.mqtt_module.output_queue)) {
    LOG_INFO("Retrying transmission of messages in the output queue. Output queue size: %d.\n",
             monitor.mqtt_module.output_queue.length);
    memset(msg, 0, MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
    memset(topic, 0, MQTT_MONITOR_TOPIC_MAX_LENGTH);
    mqtt_output_queue_extract(&monitor.mqtt_module.output_queue, msg, topic);
    publish(topic, msg);
  }

  ctimer_reset(&monitor.mqtt_module.output_queue_timer);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the publishing of an MQTT message to the subscribed topic.
 */
static void
handle_mqtt_event_publish(struct mqtt_message *msg)
{
  char start_alarm_msg[MQTT_MONITOR_INPUT_BUFFER_SIZE];

  LOG_INFO("Received %s in the topic %s.\n", msg->payload_chunk, msg->topic);

  if(monitor.state != MQTT_MONITOR_STATE_OPERATIONAL) {
    LOG_INFO("Discarding the MQTT message. The device is not in an operating state.\n");
    return;
  }

  json_message_alarm_started(start_alarm_msg, MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
  if(strcmp(start_alarm_msg, (char*)msg->payload_chunk) == 0) {
    LOG_INFO("Starting the alarm.\n");
    alarm_start(&monitor.alarm); /* There is no need to notify the collector about the state change. */
    return;
  }

  LOG_INFO("Discarding the MQTT message: bad format.\n");
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the events received during the execution of the MQTT protocol.
 *
 *          The function handles the events received during the execution of
 *          the MQTT protocol. More in details:<br>
 *          1) if the connection to the broker is successful, it changes the
 *             monitor state to MQTT_MONITOR_STATE_CONNECTED;<br>
 *          2) if the monitor gets disconnected from the broker, it changes the
 *             monitor state to MQTT_MONITOR_STATE_DISCONNECTED;<br>
 *          3) it handles the publishing of an MQTT message to the subscribed topic;<br>
 *          4) if the subscription to the topic of interest succeeds, it changes the
 *             monitor state to MQTT_MONITOR_STATE_SUBSCRIBED.
 */
static void
handle_mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void *data)
{
  switch(event) {
  case MQTT_EVENT_CONNECTED: {
    LOG_INFO("Connected to the MQTT broker.\n");
    monitor.state = MQTT_MONITOR_STATE_CONNECTED;
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    LOG_ERR("Disconnected from the MQTT broker. Reason %u.\n", *((mqtt_event_t *)data));
    monitor.state = MQTT_MONITOR_STATE_DISCONNECTED;
    process_poll(&mqtt_vital_signs_monitor);
    break;
  }
  case MQTT_EVENT_PUBLISH: {
    handle_mqtt_event_publish((struct mqtt_message*)data);
    break;
  }
  case MQTT_EVENT_SUBACK: {
#if MQTT_311
    if(((mqtt_suback_event_t *)data)->success) {
      LOG_INFO("Subscribed to the topic.\n");
      monitor.state = MQTT_MONITOR_STATE_SUBSCRIBED;
    } else {
      LOG_ERR("Failed to subscribe to the topic. Reason: %x.\n"), ((mqtt_suback_event_t *)data)->return_code);
      monitor.state = MQTT_MONITOR_STATE_CONNECTED; /* Go back to the previous state and retry. */
    }
#else
    LOG_INFO("Subscribed to the topic.\n");
    monitor.state = MQTT_MONITOR_STATE_SUBSCRIBED;
#endif
    break;
  }
  case MQTT_EVENT_UNSUBACK: {
    LOG_INFO("Unsubscribed to the topic.\n");
    break;
  }
  case MQTT_EVENT_PUBACK: {
    LOG_INFO("Publishing completed.\n");
    break;
  }
  default:
    LOG_ERR("Skipping unhandled MQTT event: %i.\n", event);
    break;
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the MQTT_MONITOR_STATE_STARTED state.
 *
 *          The function handles the MQTT_MONITOR_STATE_STARTED state,
 *          checking if the device is correctly connected to the network.
 *          If the latter is true, it changes the monitor state
 *          to MQTT_MONITOR_STATE_NETWORK_READY.
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
    monitor.state = MQTT_MONITOR_STATE_NETWORK_READY;
  } else {
    LOG_INFO("Connecting to the network.\n");
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief    Handle the MQTT_MONITOR_STATE_NETWORK_READY state.
 * \return   true if the MQTT broker parameters are valid and a connection
 *           attempt was issued, false otherwise.
 *
 *           The function handles the MQTT_MONITOR_STATE_NETWORK_READY state,
 *           initializing the monitor ID and the MQTT engine, and issuing a connection
 *           attempt to the MQTT broker. If the connection attempt is issued,
 *           i.e. if the broker parameters are valid, it changes the monitor state
 *           to MQTT_MONITOR_STATE_CONNECTING. It should be noted that the
 *           connection to the broker is finalized only when a MQTT_EVENT_CONNECTED
 *           is received, which is handled by <code>handle_mqtt_event()</code>.
 */
static bool
handle_state_network_ready(void)
{
  /* Initialize the monitor ID as the global IPv6 address. */
  uiplib_ipaddr_snprint(monitor.monitor_id,
                        MQTT_MONITOR_ID_LENGTH,
                        &(uip_ds6_get_global(ADDR_PREFERRED)->ipaddr));

  /* Initialize MQTT engine. */
  mqtt_register(&monitor.mqtt_module.connection,
                &mqtt_vital_signs_monitor,
                monitor.monitor_id,
                handle_mqtt_event,
                MQTT_MONITOR_MAX_TCP_SEGMENT_SIZE);
  LOG_INFO("MQTT engine initialized. Monitor id: %s.\n", monitor.monitor_id);

  /* Connect to the broker. */
  LOG_INFO("Connecting to the MQTT broker at %s, %d.\n", MQTT_MONITOR_BROKER_IP_ADDRESS, MQTT_MONITOR_BROKER_PORT);
  monitor.mqtt_module.status = mqtt_connect(&monitor.mqtt_module.connection,
                                            MQTT_MONITOR_BROKER_IP_ADDRESS,
                                            MQTT_MONITOR_BROKER_PORT,
                                            MQTT_MONITOR_BROKER_KEEP_ALIVE,
                                            MQTT_CLEAN_SESSION_ON);

  if(monitor.mqtt_module.status == MQTT_STATUS_ERROR) {
    LOG_ERR("Error while connecting to the MQTT broker: invalid IP address\n");
    return false;
  }

  monitor.state = MQTT_MONITOR_STATE_CONNECTING;
  return true;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief    Handle the MQTT_MONITOR_STATE_CONNECTED state.
 * \return   true if the subscription to the alarm commands topic is successful,
 *           false otherwise.
 *
 *           The function handles the MQTT_MONITOR_STATE_CONNECTED state,
 *           issuing a subscription attempt to the topic of alarm commands
 *           sent by the collector. If the attempt is issued, it changes the monitor
 *           state to MQTT_MONITOR_STATE_SUBSCRIBING. It should be noted that the
 *           subscription to the topic is finalized only when a MQTT_EVENT_SUBACK
 *           is received, which is handled by <code>handle_mqtt_event()</code>.
 */
static bool
handle_state_connected(void)
{
  /* Initialize the topics, using the monitor ID. */
  init_topics();

  /* Subscribe to the topic of alarm commands sent by the collector. */
  LOG_INFO("Subscribing to the topic %s.\n", monitor.cmd_topics.alarm_state);
  monitor.mqtt_module.status = mqtt_subscribe(&monitor.mqtt_module.connection,
                                              NULL,
                                              monitor.cmd_topics.alarm_state,
                                              MQTT_QOS_LEVEL_0);

  if(monitor.mqtt_module.status != MQTT_STATUS_OK) {
    LOG_ERR("Failed to subscribe to the topic %s.\n", monitor.cmd_topics.alarm_state);
    return false;
  }

  monitor.state = MQTT_MONITOR_STATE_SUBSCRIBING;
  return true;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the MQTT_MONITOR_STATE_SUBSCRIBED state.
 *
 *          The function handles the MQTT_MONITOR_STATE_SUBSCRIBED state,
 *          registering the monitor to the collector, starting the sensor
 *          processes and initializing the alarm system. It changes the
 *          monitor state to MQTT_MONITOR_STATE_WAITING_PATIENT_ID.
 */
static void
handle_state_subscribed(void)
{
  /* Register the monitor sending a message to the collector. */
  json_message_device_registration(monitor.output_buffers.device_registration,
                                   MQTT_MONITOR_OUTPUT_BUFFER_SIZE,
                                   monitor.monitor_id);
  publish(monitor.cmd_topics.device_registration, monitor.output_buffers.device_registration);

  /* Start the sensor processes (without starting the sampling activity). */
  start_sensor_processes();

  /* Initialize the alarm system. */
  alarm_init(&monitor.alarm);

  /*
   * From this point on, monitor.state_check_timer is used only to check
   * for eventual disconnections (state MQTT_MONITOR_STATE_DISCONNECTED).
   */
  monitor.state = MQTT_MONITOR_STATE_WAITING_PATIENT_ID;
  LOG_INFO("Waiting for a new patient ID on the serial line.\n");
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the reception of a new patient ID on the serial line.
 *
 *          The function handles the reception of a new patient ID
 *          on the serial line, sending a registration message to
 *          the collector and restarting the sampling activity
 *          of the sensor processes. It changes the
 *          monitor state to MQTT_MONITOR_STATE_OPERATIONAL.
 */
static void
handle_new_patient_ID(char *patient_id)
{
  memcpy(monitor.patient_id, patient_id, MQTT_MONITOR_PATIENT_ID_LENGTH);
  monitor.patient_id[MQTT_MONITOR_PATIENT_ID_LENGTH - 1] = '\0';
  LOG_INFO("New patient ID: %s.\n", monitor.patient_id);

  /* Register the new patient ID sending a message to the collector. */
  json_message_patient_registration(monitor.output_buffers.patient_registration,
                                    MQTT_MONITOR_OUTPUT_BUFFER_SIZE,
                                    monitor.monitor_id,
                                    monitor.patient_id);
  publish(monitor.cmd_topics.patient_registration, monitor.output_buffers.patient_registration);

  /* Start the sampling activity of the sensors. */
  start_sensor_sampling();

  monitor.state = MQTT_MONITOR_STATE_OPERATIONAL;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the button press event.
 *
 *          The function handles the button press event, turning off
 *          the alarm system and resetting the patient ID if the press
 *          duration exceeds a configured number of seconds.
 *          If the patient ID is reset, it changes the
 *          monitor state to MQTT_MONITOR_STATE_WAITING_PATIENT_ID.
 */
static void
handle_button_press(button_hal_button_t *button)
{
  LOG_INFO("Button press event: %d s.\n", button->press_duration_seconds);

  if(button->press_duration_seconds == MQTT_MONITOR_RESET_ALARM_DURATION
     || button->press_duration_seconds == MQTT_MONITOR_RESET_PATIENT_ID_DURATION) {
    bool alarm_state_changed;

    /* The alarm is stopped and an update is sent to the collector, if the alarm was turned on. */
    LOG_INFO("Resetting the alarm.\n");
    alarm_state_changed = alarm_stop(&monitor.alarm);

    if(alarm_state_changed) {
      json_message_alarm_stopped(monitor.output_buffers.alarm_state, MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
      publish(monitor.telemetry_topics.alarm_state, monitor.output_buffers.alarm_state);
    }
  }

  if(button->press_duration_seconds == MQTT_MONITOR_RESET_PATIENT_ID_DURATION) {
    LOG_INFO("Resetting the patient ID.\n");

    /* Clear the output queue, to avoid that old messages get assigned to the new patient. */
    mqtt_output_queue_init(&monitor.mqtt_module.output_queue);

    memset(monitor.patient_id, 0, MQTT_MONITOR_PATIENT_ID_LENGTH);
    stop_sensor_sampling();

    monitor.state = MQTT_MONITOR_STATE_WAITING_PATIENT_ID;
    LOG_INFO("Waiting for a new patient ID on the serial line.\n");
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Handle the reception of a sample from a sensor process.
 *
 *          The function handles the reception of a sample from a
 *          sensor process, sending it to the collector in the correct
 *          telemetry topic. If the sample is an alarming one, it turns
 *          on the alarm system and informs the collector.
 */
static void
handle_sensor_sample(process_event_t event, int sample)
{
  char *sensor;
  int min_threshold;
  int max_threshold;

  if(event == HEART_RATE_SAMPLE_EVENT) {
    sensor = "heart rate";
    min_threshold = ALARM_HEART_RATE_MIN_THRESHOLD;
    max_threshold = ALARM_HEART_RATE_MAX_THRESHOLD;
    json_message_heart_rate_sample(monitor.output_buffers.heart_rate, MQTT_MONITOR_OUTPUT_BUFFER_SIZE, sample);
    publish(monitor.telemetry_topics.heart_rate, monitor.output_buffers.heart_rate);

  } else if(event == BLOOD_PRESSURE_SAMPLE_EVENT) {
    sensor = "blood pressure";
    min_threshold = ALARM_BLOOD_PRESSURE_MIN_THRESHOLD;
    max_threshold = ALARM_BLOOD_PRESSURE_MAX_THRESHOLD;
    json_message_blood_pressure_sample(monitor.output_buffers.blood_pressure, MQTT_MONITOR_OUTPUT_BUFFER_SIZE, sample);
    publish(monitor.telemetry_topics.blood_pressure, monitor.output_buffers.blood_pressure);

  } else if(event == OXYGEN_SATURATION_SAMPLE_EVENT) {
    sensor = "oxygen saturation";
    min_threshold = ALARM_OXYGEN_SATURATION_MIN_THRESHOLD;
    max_threshold = ALARM_OXYGEN_SATURATION_MAX_THRESHOLD;
    json_message_oxygen_saturation_sample(monitor.output_buffers.oxygen_saturation, MQTT_MONITOR_OUTPUT_BUFFER_SIZE, sample);
    publish(monitor.telemetry_topics.oxygen_saturation, monitor.output_buffers.oxygen_saturation);

  } else if(event == RESPIRATION_SAMPLE_EVENT) {
    sensor = "respiration";
    min_threshold = ALARM_RESPIRATION_MIN_THRESHOLD;
    max_threshold = ALARM_RESPIRATION_MAX_THRESHOLD;
    json_message_respiration_sample(monitor.output_buffers.respiration, MQTT_MONITOR_OUTPUT_BUFFER_SIZE, sample);
    publish(monitor.telemetry_topics.respiration, monitor.output_buffers.respiration);

  } else if(event == TEMPERATURE_SAMPLE_EVENT) {
    sensor = "temperature";
    min_threshold = ALARM_TEMPERATURE_MIN_THRESHOLD;
    max_threshold = ALARM_TEMPERATURE_MAX_THRESHOLD;
    json_message_temperature_sample(monitor.output_buffers.temperature, MQTT_MONITOR_OUTPUT_BUFFER_SIZE, sample);
    publish(monitor.telemetry_topics.temperature, monitor.output_buffers.temperature);
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
      json_message_alarm_started(monitor.output_buffers.alarm_state, MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
      publish(monitor.telemetry_topics.alarm_state, monitor.output_buffers.alarm_state);
    }
  }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Initialize the state and timers of the monitor.
 */
static void
init_monitor()
{
  monitor.state = MQTT_MONITOR_STATE_STARTED;

  /* Initialize the periodic timer to check the internal state. */
  monitor.state_check_interval = MQTT_MONITOR_STATE_CHECK_INTERVAL*CLOCK_SECOND;
  etimer_set(&monitor.state_check_timer, monitor.state_check_interval);

  /* Initialize the message output queue and the periodic timer to send its messages. */
  mqtt_output_queue_init(&monitor.mqtt_module.output_queue);
  monitor.mqtt_module.output_queue_timer_interval = MQTT_MONITOR_OUTPUT_QUEUE_SEND_INTERVAL*CLOCK_SECOND;
  ctimer_set(&monitor.mqtt_module.output_queue_timer,
             monitor.mqtt_module.output_queue_timer_interval,
             retry_message_transmission,
             NULL);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief   Terminate the monitor stopping the timers, the processes simulating
 *          the sensors and the alarm system.
 */
static void
finish_monitor()
{
  etimer_stop(&monitor.state_check_timer);
  ctimer_stop(&monitor.mqtt_module.output_queue_timer);
  stop_sensor_sampling();
  stop_sensor_processes();
  alarm_stop(&monitor.alarm);
}
/*---------------------------------------------------------------------------*/
/*
 * Process simulating a vital signs monitor using the MQTT protocol
 * to exchange data with a collector.
 * The process establishes an MQTT connection with a configured broker and:
 * 1) sends telemetry data coming from a set of simulated sensor processes;
 * 2) receives commands able to trigger the alarm system.
 * The monitor requires a patient ID in order to be fully operational, which can
 * be passed via the serial line at startup. The patient ID can be reset and
 * re-inserted pressing the button of the device for at least 10 seconds.
 * If the alarm system is triggered, the alarm state can be turned off by
 * pressing the same button for at least 5 seconds.
 */
PROCESS(mqtt_vital_signs_monitor, "MQTT vital signs monitor");
AUTOSTART_PROCESSES(&mqtt_vital_signs_monitor);

PROCESS_THREAD(mqtt_vital_signs_monitor, event, data)
{
  PROCESS_BEGIN();
  LOG_INFO("Process started.\n");
  init_monitor();

  while(true) {
    PROCESS_WAIT_EVENT();

    if(event == PROCESS_EVENT_TIMER || event == PROCESS_EVENT_POLL) {
      if(monitor.state == MQTT_MONITOR_STATE_STARTED) {
        handle_state_started();
      }

      if(monitor.state == MQTT_MONITOR_STATE_NETWORK_READY) {
        bool success = handle_state_network_ready();
        if(!success) {
          break;
        }
      }

      /* MQTT_MONITOR_STATE_CONNECTED is set by handle_mqtt_event(). */
      if(monitor.state == MQTT_MONITOR_STATE_CONNECTED) {
        bool success = handle_state_connected();
        if(!success) {
          break;
        }
      }

      /* MQTT_MONITOR_STATE_SUBSCRIBED is set by handle_mqtt_event(). */
      if(monitor.state == MQTT_MONITOR_STATE_SUBSCRIBED) {
        handle_state_subscribed();
      }

      /* MQTT_MONITOR_STATE_DISCONNECTED is set by handle_mqtt_event(). */
      if(monitor.state == MQTT_MONITOR_STATE_DISCONNECTED) {
        break;
      }

      etimer_reset(&monitor.state_check_timer);
      continue;
    }

    if(event == serial_line_event_message && monitor.state == MQTT_MONITOR_STATE_WAITING_PATIENT_ID) {
      handle_new_patient_ID((char*)data);
      continue;
    }

    if(event == button_hal_periodic_event && monitor.state == MQTT_MONITOR_STATE_OPERATIONAL) {
      handle_button_press((button_hal_button_t *)data);
      continue;
    }

    if(sensor_sample_event(event) && monitor.state == MQTT_MONITOR_STATE_OPERATIONAL) {
      handle_sensor_sample(event, *((int *)data));
      continue;
    }
  }

  finish_monitor();
  LOG_INFO("Stopping the process.\n");
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
