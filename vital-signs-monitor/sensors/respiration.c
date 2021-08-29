/**
 * \file
 *         Respiration sensor simulator implementation
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup respiration
 * @{
 */

#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./sensor.h"
#include "./utils/sensor-constants.h"
#include "./utils/prng.h"

#define LOG_MODULE "Respiration sensor"
#define LOG_LEVEL LOG_LEVEL_RESPIRATION_SENSOR

process_event_t RESPIRATION_SAMPLE_EVENT;
process_event_t RESPIRATION_START_SAMPLING_EVENT;
process_event_t RESPIRATION_STOP_SAMPLING_EVENT;

/*---------------------------------------------------------------------------*/
/*
 * Process simulating the sampling made by a respiration sensor.
 * The sampling can be started and stopped by sending the RESPIRATION_START_SAMPLING_EVENT
 * and RESPIRATION_STOP_SAMPLING_EVENT, respectively. The occurrence of a new sample
 * is signaled by sending the RESPIRATION_SAMPLE_EVENT to the subscribed process.
 */
PROCESS(respiration_sensor_process, "Respiration sensor process");

PROCESS_THREAD(respiration_sensor_process, event, data) {
  static struct sensor respiration_sensor = {.sampling_interval = RESPIRATION_SAMPLING_INTERVAL*CLOCK_SECOND};
  PROCESS_BEGIN();

  LOG_INFO("Process started.\n");
  RESPIRATION_SAMPLE_EVENT = process_alloc_event();
  RESPIRATION_START_SAMPLING_EVENT = process_alloc_event();
  RESPIRATION_STOP_SAMPLING_EVENT = process_alloc_event();

  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(event == RESPIRATION_START_SAMPLING_EVENT);
    respiration_sensor.subscriber = (struct process *) data;
    LOG_INFO("Starting sampling with interval %d s. Subscribed process: %s \n",
             RESPIRATION_SAMPLING_INTERVAL,
             respiration_sensor.subscriber->name);

    /* Initialize the respiration value. */
    respiration_sensor.last_sample = prng_rand(RESPIRATION_LOWER_BOUND, RESPIRATION_UPPER_BOUND);
    etimer_set(&respiration_sensor.sampling_timer, respiration_sensor.sampling_interval);

    while(true) {
      PROCESS_WAIT_EVENT_UNTIL(event == PROCESS_EVENT_TIMER || event == RESPIRATION_STOP_SAMPLING_EVENT);

      if(event == PROCESS_EVENT_TIMER) {
        respiration_sensor.last_sample = sensor_generate_sample(respiration_sensor.last_sample,
                                                                RESPIRATION_DEVIATION,
                                                                RESPIRATION_LOWER_BOUND,
                                                                RESPIRATION_UPPER_BOUND);
        LOG_INFO("New sample: %d %s.\n", respiration_sensor.last_sample, RESPIRATION_UNIT);
        process_post(respiration_sensor.subscriber, RESPIRATION_SAMPLE_EVENT, &respiration_sensor.last_sample);
        etimer_reset(&respiration_sensor.sampling_timer);
        continue;
      }

      if(event == RESPIRATION_STOP_SAMPLING_EVENT) {
        LOG_INFO("Stopping sampling.\n");
        etimer_stop(&respiration_sensor.sampling_timer);
        break;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
