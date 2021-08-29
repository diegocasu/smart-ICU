/**
 * \file
 *         Oxygen saturation sensor simulator implementation
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup oxygen-saturation
 * @{
 */

#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./sensor.h"
#include "./utils/sensor-constants.h"
#include "./utils/prng.h"

#define LOG_MODULE "Oxygen saturation sensor"
#define LOG_LEVEL LOG_LEVEL_OXYGEN_SATURATION_SENSOR

process_event_t OXYGEN_SATURATION_SAMPLE_EVENT;
process_event_t OXYGEN_SATURATION_START_SAMPLING_EVENT;
process_event_t OXYGEN_SATURATION_STOP_SAMPLING_EVENT;

/*---------------------------------------------------------------------------*/
/*
 * Process simulating the sampling made by an oxygen saturation sensor.
 * The sampling can be started and stopped by sending the OXYGEN_SATURATION_START_SAMPLING_EVENT
 * and OXYGEN_SATURATION_STOP_SAMPLING_EVENT, respectively. The occurrence of a new sample
 * is signaled by sending the OXYGEN_SATURATION_SAMPLE_EVENT to the subscribed process.
 */
PROCESS(oxygen_saturation_sensor_process, "Oxygen saturation sensor process");

PROCESS_THREAD(oxygen_saturation_sensor_process, event, data) {
  static struct sensor oxygen_saturation_sensor = {.sampling_interval = OXYGEN_SATURATION_SAMPLING_INTERVAL*CLOCK_SECOND};
  PROCESS_BEGIN();

  LOG_INFO("Process started.\n");
  OXYGEN_SATURATION_SAMPLE_EVENT = process_alloc_event();
  OXYGEN_SATURATION_START_SAMPLING_EVENT = process_alloc_event();
  OXYGEN_SATURATION_STOP_SAMPLING_EVENT = process_alloc_event();

  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(event == OXYGEN_SATURATION_START_SAMPLING_EVENT);
    oxygen_saturation_sensor.subscriber = (struct process *) data;
    LOG_INFO("Starting sampling with interval %d s. Subscribed process: %s \n",
             OXYGEN_SATURATION_SAMPLING_INTERVAL,
             oxygen_saturation_sensor.subscriber->name);

    /* Initialize the oxygen saturation value. */
    oxygen_saturation_sensor.last_sample = prng_rand(OXYGEN_SATURATION_LOWER_BOUND, OXYGEN_SATURATION_UPPER_BOUND);
    etimer_set(&oxygen_saturation_sensor.sampling_timer, oxygen_saturation_sensor.sampling_interval);

    while(true) {
      PROCESS_WAIT_EVENT_UNTIL(event == PROCESS_EVENT_TIMER || event == OXYGEN_SATURATION_STOP_SAMPLING_EVENT);

      if(event == PROCESS_EVENT_TIMER) {
        oxygen_saturation_sensor.last_sample = sensor_generate_sample(oxygen_saturation_sensor.last_sample,
                                                                      OXYGEN_SATURATION_DEVIATION,
                                                                      OXYGEN_SATURATION_LOWER_BOUND,
                                                                      OXYGEN_SATURATION_UPPER_BOUND);
        LOG_INFO("New sample: %d %s.\n", oxygen_saturation_sensor.last_sample, OXYGEN_SATURATION_UNIT);
        process_post(oxygen_saturation_sensor.subscriber, OXYGEN_SATURATION_SAMPLE_EVENT,
                     &oxygen_saturation_sensor.last_sample);
        etimer_reset(&oxygen_saturation_sensor.sampling_timer);
        continue;
      }

      if(event == OXYGEN_SATURATION_STOP_SAMPLING_EVENT) {
        LOG_INFO("Stopping sampling.\n");
        etimer_stop(&oxygen_saturation_sensor.sampling_timer);
        break;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
