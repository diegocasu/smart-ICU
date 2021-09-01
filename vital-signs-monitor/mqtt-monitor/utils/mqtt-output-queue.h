/**
 * \file
 *         Header file for the MQTT message output queue
 * \author
 *         Diego Casu
 */

/**
 * \defgroup mqtt-output-queue MQTT message output queue
 * @{
 *
 * The mqtt-output-queue module provides the implementation of a simple fixed-size FIFO queue.
 * The queue is organized as a circular buffer and does not overwrite old messages if full,
 * i.e. messages are not inserted if the queue is full.
 */

#ifndef SMART_ICU_MQTT_OUTPUT_QUEUE_H
#define SMART_ICU_MQTT_OUTPUT_QUEUE_H

#include <stdbool.h>
#include "./mqtt-monitor-constants.h"

/*
 * Structure representing an MQTT message queue.
 * For each message, the relative publishing topic is saved: given a message in the position i
 * of msg_queue, the associated topic is saved in the same position i of topic_queue.
 */
struct mqtt_output_queue {
  char msg_queue[MQTT_MONITOR_OUTPUT_QUEUE_SIZE][MQTT_MONITOR_OUTPUT_BUFFER_SIZE];
  char topic_queue[MQTT_MONITOR_OUTPUT_QUEUE_SIZE][MQTT_MONITOR_TOPIC_MAX_LENGTH];
  int insert_index;
  int extract_index;
  int length;
};

/**
 * \brief         Test if the given queue is empty.
 * \param queue   A pointer to the queue to be tested.
 * \return        true if the queue is empty, false otherwise.
 *
 *                The function tests if the given queue is empty.
 *                A queue is empty if its length is 0.
 */
bool mqtt_output_queue_is_empty(struct mqtt_output_queue *queue);

/**
 * \brief         Test if the given queue is full.
 * \param queue   A pointer to the queue to be tested.
 * \return        true if the queue is full, false otherwise.
 *
 *                The function tests if the given queue is full.
 *                A queue is full if its length is equal to MQTT_MONITOR_OUTPUT_QUEUE_SIZE.
 */
bool mqtt_output_queue_is_full(struct mqtt_output_queue *queue);

/**
 * \brief         Initialize a message queue.
 * \param queue   A pointer to the queue to be initialized.
 *
 *                The function initializes a message queue setting the values
 *                of <code>insert_index</code>, <code>extract_index</code> and <code>length</code>
 *                to 0. If the message queue contains messages at the time
 *                this function is called, they are not deleted: the latter will
 *                happen when an insertion in their positions will be performed.
 */
void mqtt_output_queue_init(struct mqtt_output_queue *queue);

/**
 * \brief         Insert a message and the relative topic in the given queue.
 * \param queue   A pointer to the queue.
 * \param msg     A pointer to the message to be inserted.
 * \param topic   A pointer to the topic of the message.
 * \return        true if the insertion succeeded, false otherwise.
 *
 *                The function inserts a message and the relative topic in the given queue.
 *                The insertion succeeds only if the queue is not full.
 *                The message must be at most MQTT_MONITOR_OUTPUT_BUFFER_SIZE - 1 long and the topic
 *                must be at most MQTT_MONITOR_TOPIC_MAX_LENGTH - 1 long, otherwise they are truncated.
 *                The inserted messages and topics are always null terminated.
 */
bool mqtt_output_queue_insert(struct mqtt_output_queue *queue, char *msg, char *topic);

/**
 * \brief         Extract a message and the relative topic from the given queue.
 * \param queue   A pointer to the queue.
 * \param msg     A pointer to the buffer that will hold the message.
 * \param topic   A pointer to the buffer that will hold the topic of the message.
 * \return        true if the extraction succeeded, false otherwise.
 *
 *                The function extracts a message and the relative topic from the given queue.
 *                The extraction succeeds only if the queue is not empty.
 *                The message buffer must be at least MQTT_MONITOR_OUTPUT_BUFFER_SIZE long and the topic
 *                buffer must be at least MQTT_MONITOR_TOPIC_MAX_LENGTH long, otherwise the strings
 *                could be truncated. The extracted messages and topics are always null terminated.
 */
bool mqtt_output_queue_extract(struct mqtt_output_queue *queue, char *msg, char *topic);

#endif /* SMART_ICU_MQTT_OUTPUT_QUEUE_H */
/** @} */
