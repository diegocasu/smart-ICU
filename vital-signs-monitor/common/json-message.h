/**
 * \file
 *         Header file for the functions generating JSON payloads for MQTT/CoAP messages.
 * \author
 *         Diego Casu
 */

/**
 * \defgroup json-message MQTT/CoAP JSON payloads
 * @{
 *
 * The json-message module provides functions to generate the JSON payloads of
 * MQTT/CoAP messages exchanged with the collector.
 */

#ifndef SMART_ICU_JSON_MESSAGE_H
#define SMART_ICU_JSON_MESSAGE_H

/**
 * \brief                  Generate a device registration message.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param monitor_id       The ID of the vital signs monitor.
 *
 *                         The function generates a device registration message for
 *                         the specified vital signs monitor.
 */
void json_message_device_registration(char *message_buffer, size_t size, char *monitor_id);

/**
 * \brief                  Generate a patient registration message.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param monitor_id       The ID of the vital signs monitor.
 *                         If NULL, it is not inserted in the message.
 * \param patient_id       The ID of the patient.
 *
 *                         The function generates a patient registration message for
 *                         the specified vital signs monitor and patient.
 */
void json_message_patient_registration(char *message_buffer, size_t size, char *monitor_id, char *patient_id);

/**
 * \brief                  Generate a message informing that an alarm has been turned on.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 *
 *                         The function generates a message informing that an alarm
 *                         has been turned on.
 */
void json_message_alarm_started(char *message_buffer, size_t size);

/**
 * \brief                  Generate a message informing that an alarm has been turned off.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 *
 *                         The function generates a message informing that an alarm
 *                         has been turned off.
 */
void json_message_alarm_stopped(char *message_buffer, size_t size);

/**
 * \brief                  Generate a message containing a heart rate sample.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param sample           The heart rate sample.
 *
 *                         The function generates a message containing a heart rate sample,
 *                         together with its measurement unit and the current timestamp.
 */
void json_message_heart_rate_sample(char *message_buffer, size_t size, int sample);

/**
 * \brief                  Generate a message containing a blood pressure sample.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param sample           The blood pressure sample.
 *
 *                         The function generates a message containing a blood pressure sample,
 *                         together with its measurement unit and the current timestamp.
 */
void json_message_blood_pressure_sample(char *message_buffer, size_t size, int sample);

/**
 * \brief                  Generate a message containing an oxygen saturation sample.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param sample           The oxygen saturation sample.
 *
 *                         The function generates a message containing an oxygen saturation sample,
 *                         together with its measurement unit and the current timestamp.
 */
void json_message_oxygen_saturation_sample(char *message_buffer, size_t size, int sample);

/**
 * \brief                  Generate a message containing a respiration sample.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param sample           The respiration sample.
 *
 *                         The function generates a message containing a respiration sample,
 *                         together with its measurement unit and the current timestamp.
 */
void json_message_respiration_sample(char *message_buffer, size_t size, int sample);

/**
 * \brief                  Generate a message containing a temperature sample.
 * \param message_buffer   A pointer to the buffer that will store the message.
 * \param size             The size of the buffer.
 * \param sample           The temperature sample.
 *
 *                         The function generates a message containing a temperature sample,
 *                         together with its measurement unit and the current timestamp.
 */
void json_message_temperature_sample(char *message_buffer, size_t size, int sample);

#endif /* SMART_ICU_JSON_MESSAGE_H */
/** @} */
