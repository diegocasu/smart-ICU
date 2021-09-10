/**
 * \file
 *         Implementation of the heart rate sensor simulator
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup heart-rate
 * @{
 */

#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./sensor.h"
#include "./utils/sensor-constants.h"

#define LOG_MODULE "Heart rate sensor"
#define LOG_LEVEL LOG_LEVEL_HEART_RATE_SENSOR

process_event_t HEART_RATE_SAMPLE_EVENT;
process_event_t HEART_RATE_START_SAMPLING_EVENT;
process_event_t HEART_RATE_STOP_SAMPLING_EVENT;

/*---------------------------------------------------------------------------*/
/*
 * Process simulating the sampling made by a heart rate sensor.
 * The sampling can be started and stopped by sending the HEART_RATE_START_SAMPLING_EVENT
 * and HEART_RATE_STOP_SAMPLING_EVENT, respectively. The occurrence of a new sample
 * is signaled by sending the HEART_RATE_SAMPLE_EVENT to the subscribed process.
 */
PROCESS(heart_rate_sensor_process, "Heart rate sensor process");

PROCESS_THREAD(heart_rate_sensor_process, event, data)
{
  static struct sensor heart_rate_sensor = {.sampling_interval = HEART_RATE_SAMPLING_INTERVAL*CLOCK_SECOND};
  PROCESS_BEGIN();

  LOG_INFO("Process started.\n");
  HEART_RATE_SAMPLE_EVENT = process_alloc_event();
  HEART_RATE_START_SAMPLING_EVENT = process_alloc_event();
  HEART_RATE_STOP_SAMPLING_EVENT = process_alloc_event();

  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(event == HEART_RATE_START_SAMPLING_EVENT);
    heart_rate_sensor.subscriber = (struct process *)data;
    LOG_INFO("Starting sampling with interval %d s. Subscribed process: %s.\n",
             HEART_RATE_SAMPLING_INTERVAL,
             heart_rate_sensor.subscriber->name);

    /* Initialize the heart rate value. */
    heart_rate_sensor.last_sample = sensor_rand_int(HEART_RATE_LOWER_BOUND, HEART_RATE_UPPER_BOUND);
    etimer_set(&heart_rate_sensor.sampling_timer, heart_rate_sensor.sampling_interval);

    while(true) {
      PROCESS_WAIT_EVENT_UNTIL(event == PROCESS_EVENT_TIMER || event == HEART_RATE_STOP_SAMPLING_EVENT);

      if(event == PROCESS_EVENT_TIMER) {
        heart_rate_sensor.last_sample = sensor_generate_sample(heart_rate_sensor.last_sample,
                                                               HEART_RATE_DEVIATION,
                                                               HEART_RATE_LOWER_BOUND,
                                                               HEART_RATE_UPPER_BOUND);
        LOG_INFO("New sample: %d %s.\n", heart_rate_sensor.last_sample, HEART_RATE_UNIT);
        process_post(heart_rate_sensor.subscriber, HEART_RATE_SAMPLE_EVENT, &heart_rate_sensor.last_sample);
        etimer_reset(&heart_rate_sensor.sampling_timer);
        continue;
      }

      if(event == HEART_RATE_STOP_SAMPLING_EVENT) {
        LOG_INFO("Stopping sampling.\n");
        etimer_stop(&heart_rate_sensor.sampling_timer);
        break;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
