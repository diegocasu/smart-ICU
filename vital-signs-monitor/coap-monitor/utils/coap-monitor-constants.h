/**
 * \file
 *         Constants used by the CoAP vital signs monitor
 * \author
 *         Diego Casu
 */

/**
 * \defgroup coap-monitor-constants CoAP monitor constants
 * @{
 *
 * Constants used by the CoAP vital signs monitor to manage CoAP parameters,
 * connect to the collector, instantiate buffers and represent internal states.
 */

#ifndef SMART_ICU_COAP_MONITOR_CONSTANTS_H
#define SMART_ICU_COAP_MONITOR_CONSTANTS_H

/* Helpers to convert integers to strings in the preprocessor directives. */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* Collector constants. */
#define COAP_MONITOR_COLLECTOR_IP_ADDRESS                     "fd00::1"
#define COAP_MONITOR_COLLECTOR_PORT                           5683
#define COAP_MONITOR_COLLECTOR_REGISTERED_MONITORS_RESOURCE   "/registeredMonitors"
#define COAP_MONITOR_COLLECTOR_ENDPOINT                       "coap://[" COAP_MONITOR_COLLECTOR_IP_ADDRESS "]:" STR(COAP_MONITOR_COLLECTOR_PORT)

/* CoAP monitor constants. */
#define COAP_MONITOR_ID_LENGTH                                46 /* The maximum length of a monitor ID (an IPv6 address). */
#define COAP_MONITOR_NETWORK_CHECK_INTERVAL                   1  /* Interval in seconds used by the periodic timer to check
                                                                   if the network connectivity has been established. */
#define COAP_MONITOR_OUTPUT_BUFFER_SIZE                       256 /* Size of the CoAP output buffer. */
#define COAP_MONITOR_PATIENT_ID_LENGTH                        10 /* The maximum length of a patient ID. */
#define COAP_MONITOR_RESET_PATIENT_ID_DURATION                10 /* Time in seconds for which the button must be kept pressed to reset the patient ID. */
#define COAP_MONITOR_RESET_ALARM_DURATION                     5  /* Time in seconds for which the button must be kept pressed to reset the alarm state. */

/* CoAP monitor internal states. */
#define COAP_MONITOR_STATE_STARTED                            0 /* Initial state. */
#define COAP_MONITOR_STATE_NETWORK_READY                      1 /* Network is initialized. */
#define COAP_MONITOR_STATE_WAITING_PATIENT_ID                 2 /* Waiting for a patient ID as input. */
#define COAP_MONITOR_STATE_OPERATIONAL                        3 /* Ready for working. */
#define COAP_MONITOR_STATE_REGISTRATION_FAILED                4 /* Failed to register to the collector. */

/* CoAP monitor resources. */
#define COAP_MONITOR_RESOURCE_OUTPUT_BUFFER_SIZE              256                             /* Size of the output buffer storing the value of a resource. */
#define COAP_MONITOR_REGISTERED_PATIENT_RESOURCE              "registeredPatient"             /* Resource holding the ID of the patient attached to the monitor. */
#define COAP_MONITOR_ALARM_STATE_RESOURCE                     "patientState/alarmState"       /* Resource holding the state of the alarm system. */
#define COAP_MONITOR_HEART_RATE_RESOURCE                      "patientState/heartRate"        /* Resource holding the last sampled value of the heart rate. */
#define COAP_MONITOR_BLOOD_PRESSURE_RESOURCE                  "patientState/bloodPressure"    /* Resource holding the last sampled value of the blood pressure. */
#define COAP_MONITOR_TEMPERATURE_RESOURCE                     "patientState/temperature"      /* Resource holding the last sampled value of the temperature. */
#define COAP_MONITOR_RESPIRATION_RESOURCE                     "patientState/respiration"      /* Resource holding the last sampled value of the respiration. */
#define COAP_MONITOR_OXYGEN_SATURATION_RESOURCE               "patientState/oxygenSaturation" /* Resource holding the last sampled value of the oxygen saturation. */

#endif /* SMART_ICU_COAP_MONITOR_CONSTANTS_H */
/** @} */
