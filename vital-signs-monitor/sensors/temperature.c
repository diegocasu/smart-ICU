/**
 * \file
 *         Implementation of the temperature sensor simulator
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup temperature
 * @{
 */

#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./sensor.h"
#include "./utils/sensor-constants.h"
#include "./utils/prng.h"

#define LOG_MODULE "Temperature sensor"
#define LOG_LEVEL LOG_LEVEL_TEMPERATURE_SENSOR

process_event_t TEMPERATURE_SAMPLE_EVENT;
process_event_t TEMPERATURE_START_SAMPLING_EVENT;
process_event_t TEMPERATURE_STOP_SAMPLING_EVENT;

/*---------------------------------------------------------------------------*/
/*
 * Process simulating the sampling made by a temperature sensor.
 * The sampling can be started and stopped by sending the TEMPERATURE_START_SAMPLING_EVENT
 * and TEMPERATURE_STOP_SAMPLING_EVENT, respectively. The occurrence of a new sample
 * is signaled by sending the TEMPERATURE_SAMPLE_EVENT to the subscribed process.
 */
PROCESS(temperature_sensor_process, "Temperature sensor process");

PROCESS_THREAD(temperature_sensor_process, event, data)
{
  static struct sensor temperature_sensor = {.sampling_interval = TEMPERATURE_SAMPLING_INTERVAL*CLOCK_SECOND};
  PROCESS_BEGIN();

  LOG_INFO("Process started.\n");
  TEMPERATURE_SAMPLE_EVENT = process_alloc_event();
  TEMPERATURE_START_SAMPLING_EVENT = process_alloc_event();
  TEMPERATURE_STOP_SAMPLING_EVENT = process_alloc_event();

  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(event == TEMPERATURE_START_SAMPLING_EVENT);
    temperature_sensor.subscriber = (struct process *)data;
    LOG_INFO("Starting sampling with interval %d s. Subscribed process: %s.\n",
             TEMPERATURE_SAMPLING_INTERVAL,
             temperature_sensor.subscriber->name);

    /* Initialize the temperature value. */
    temperature_sensor.last_sample = prng_rand(TEMPERATURE_LOWER_BOUND, TEMPERATURE_UPPER_BOUND);
    etimer_set(&temperature_sensor.sampling_timer, temperature_sensor.sampling_interval);

    while(true) {
      PROCESS_WAIT_EVENT_UNTIL(event == PROCESS_EVENT_TIMER || event == TEMPERATURE_STOP_SAMPLING_EVENT);

      if(event == PROCESS_EVENT_TIMER) {
        temperature_sensor.last_sample = sensor_generate_sample(temperature_sensor.last_sample,
                                                                TEMPERATURE_DEVIATION,
                                                                TEMPERATURE_LOWER_BOUND,
                                                                TEMPERATURE_UPPER_BOUND);
        LOG_INFO("New sample: %d %s.\n", temperature_sensor.last_sample, TEMPERATURE_UNIT);
        process_post(temperature_sensor.subscriber, TEMPERATURE_SAMPLE_EVENT, &temperature_sensor.last_sample);
        etimer_reset(&temperature_sensor.sampling_timer);
        continue;
      }

      if(event == TEMPERATURE_STOP_SAMPLING_EVENT) {
        LOG_INFO("Stopping sampling.\n");
        etimer_stop(&temperature_sensor.sampling_timer);
        break;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
