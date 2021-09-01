/**
 * \file
 *         MQTT message output queue implementation
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup mqtt-output-queue
 * @{
 */

#include <string.h>
#include "./mqtt-output-queue.h"

/*---------------------------------------------------------------------------*/
bool
mqtt_output_queue_is_empty(struct mqtt_output_queue *queue)
{
  if(queue->length == 0) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
bool
mqtt_output_queue_is_full(struct mqtt_output_queue *queue)
{
  if(queue->length == MQTT_MONITOR_OUTPUT_QUEUE_SIZE) {
    return true;
  }
  return false;
}
/*---------------------------------------------------------------------------*/
void
mqtt_output_queue_init(struct mqtt_output_queue *queue)
{
  queue->insert_index = 0;
  queue->extract_index = 0;
  queue->length = 0;
}
/*---------------------------------------------------------------------------*/
bool mqtt_output_queue_insert(struct mqtt_output_queue *queue, char *msg, char *topic)
{
  if(mqtt_output_queue_is_full(queue)) {
    return false;
  }

  memset(queue->msg_queue[queue->insert_index], 0, MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
  memset(queue->topic_queue[queue->insert_index], 0, MQTT_MONITOR_TOPIC_MAX_LENGTH);

  memcpy(queue->msg_queue[queue->insert_index], msg, MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
  queue->msg_queue[queue->insert_index][MQTT_MONITOR_OUTPUT_BUFFER_SIZE - 1] = '\0';

  memcpy(queue->topic_queue[queue->insert_index], topic, MQTT_MONITOR_TOPIC_MAX_LENGTH);
  queue->msg_queue[queue->insert_index][MQTT_MONITOR_TOPIC_MAX_LENGTH - 1] = '\0';

  queue->length = queue->length + 1;
  queue->insert_index = (queue->insert_index + 1) % MQTT_MONITOR_OUTPUT_QUEUE_SIZE;

  return true;
}
/*---------------------------------------------------------------------------*/
bool mqtt_output_queue_extract(struct mqtt_output_queue *queue, char *msg, char *topic)
{
  if(mqtt_output_queue_is_empty(queue)) {
    return false;
  }

  memcpy(msg, queue->msg_queue[queue->extract_index], MQTT_MONITOR_OUTPUT_BUFFER_SIZE);
  msg[MQTT_MONITOR_OUTPUT_BUFFER_SIZE - 1] = '\0';

  memcpy(topic, queue->topic_queue[queue->extract_index], MQTT_MONITOR_TOPIC_MAX_LENGTH);
  topic[MQTT_MONITOR_TOPIC_MAX_LENGTH - 1] = '\0';

  queue->length = queue->length - 1;
  queue->extract_index = (queue->extract_index + 1) % MQTT_MONITOR_OUTPUT_QUEUE_SIZE;

  return true;
}
/*---------------------------------------------------------------------------*/
/** @} */
