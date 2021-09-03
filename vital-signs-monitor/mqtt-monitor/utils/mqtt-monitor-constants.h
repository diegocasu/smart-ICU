/**
 * \file
 *         Constants used by the MQTT vital signs monitor
 * \author
 *         Diego Casu
 */

/**
 * \defgroup mqtt-monitor-constants MQTT monitor constants
 * @{
 *
 * Constants used by the MQTT vital signs monitor to manage MQTT parameters,
 * connect to the broker, instantiate buffers and represent internal states.
 */

#ifndef SMART_ICU_MQTT_MONITOR_CONSTANTS_H
#define SMART_ICU_MQTT_MONITOR_CONSTANTS_H

/* MQTT broker constants. */
#define MQTT_MONITOR_BROKER_IP_ADDRESS                   "fd00::1" /* IPv6 address of the MQTT broker. */
#define MQTT_MONITOR_BROKER_PORT                         1883      /* Port of the MQTT broker. */
#define MQTT_MONITOR_BROKER_KEEP_ALIVE                   60        /* Keep-alive for the MQTT connection. */

/* MQTT monitor (MQTT client) constants. */
#define MQTT_MONITOR_ID_LENGTH                           46  /* The maximum length of a monitor ID (an IPv6 address). */
#define MQTT_MONITOR_STATE_CHECK_INTERVAL                1   /* Interval in seconds used by the periodic timer to check the internal state. */
#define MQTT_MONITOR_MAX_TCP_SEGMENT_SIZE                256 /* Maximum TCP segment size for the outgoing segments. */
#define MQTT_MONITOR_INPUT_BUFFER_SIZE                   32  /* Size of the MQTT input buffer. */
#define MQTT_MONITOR_OUTPUT_BUFFER_SIZE                  256 /* Size of the MQTT output buffer. */
#define MQTT_MONITOR_TOPIC_MAX_LENGTH                    128 /* Maximum length of a topic label. */
#define MQTT_MONITOR_OUTPUT_QUEUE_SIZE                   10  /* Size of the output queue used to store MQTT messages. */
#define MQTT_MONITOR_OUTPUT_QUEUE_SEND_INTERVAL          5   /* Interval in seconds used by the periodic timer to empty the output queue. */
#define MQTT_MONITOR_PATIENT_ID_LENGTH                   10 /* The maximum length of a patient ID. */
#define MQTT_MONITOR_RESET_PATIENT_ID_DURATION           10 /* Time in seconds for which the button must be kept pressed to reset the patient ID. */
#define MQTT_MONITOR_RESET_ALARM_DURATION                5  /* Time in seconds for which the button must be kept pressed to reset the alarm state. */

/* MQTT monitor internal states. */
#define MQTT_MONITOR_STATE_STARTED                       0 /* Initial state. */
#define MQTT_MONITOR_STATE_NETWORK_READY                 1 /* Network is initialized. */
#define MQTT_MONITOR_STATE_CONNECTING                    2 /* Connecting to the MQTT broker. */
#define MQTT_MONITOR_STATE_CONNECTED                     3 /* Successfully connected to the broker. */
#define MQTT_MONITOR_STATE_SUBSCRIBING                   4 /* Subscribing to the topics of interest. */
#define MQTT_MONITOR_STATE_SUBSCRIBED                    5 /* Subscribed to the topics of interest. */
#define MQTT_MONITOR_STATE_DISCONNECTED                  6 /* Disconnected from the MQTT broker. */
#define MQTT_MONITOR_STATE_WAITING_PATIENT_ID            7 /* Waiting for a patient ID as input. */
#define MQTT_MONITOR_STATE_OPERATIONAL                   8 /* Ready for working. */

/* MQTT command and telemetry topics. */
#define MQTT_MONITOR_CMD_TOPIC_ALARM_STATE               "cmd/smartICU/%s/patient-state/alarm-state"
#define MQTT_MONITOR_CMD_TOPIC_MONITOR_REGISTRATION       "cmd/smartICU/collector/monitor-registration"
#define MQTT_MONITOR_CMD_TOPIC_PATIENT_REGISTRATION      "cmd/smartICU/collector/patient-registration"
#define MQTT_MONITOR_TELEMETRY_TOPIC_HEART_RATE          "telemetry/smartICU/%s/patient-state/heart-rate"
#define MQTT_MONITOR_TELEMETRY_TOPIC_BLOOD_PRESSURE      "telemetry/smartICU/%s/patient-state/blood-pressure"
#define MQTT_MONITOR_TELEMETRY_TOPIC_TEMPERATURE         "telemetry/smartICU/%s/patient-state/temperature"
#define MQTT_MONITOR_TELEMETRY_TOPIC_RESPIRATION         "telemetry/smartICU/%s/patient-state/respiration"
#define MQTT_MONITOR_TELEMETRY_TOPIC_OXYGEN_SATURATION   "telemetry/smartICU/%s/patient-state/oxygen-saturation"
#define MQTT_MONITOR_TELEMETRY_TOPIC_ALARM_STATE         "telemetry/smartICU/%s/patient-state/alarm-state"

#endif /* SMART_ICU_MQTT_MONITOR_CONSTANTS_H */
/** @} */
