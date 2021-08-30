/**
 * \file
 *         Blood pressure sensor simulator implementation
 * \author
 *         Diego Casu
 */

/**
 * \addtogroup blood-pressure
 * @{
 */

#include <stdbool.h>
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/log.h"
#include "./sensor.h"
#include "./utils/sensor-constants.h"
#include "./utils/prng.h"

#define LOG_MODULE "Blood pressure sensor"
#define LOG_LEVEL LOG_LEVEL_BLOOD_PRESSURE_SENSOR

process_event_t BLOOD_PRESSURE_SAMPLE_EVENT;
process_event_t BLOOD_PRESSURE_START_SAMPLING_EVENT;
process_event_t BLOOD_PRESSURE_STOP_SAMPLING_EVENT;

/*---------------------------------------------------------------------------*/
/*
 * Process simulating the sampling made by a blood pressure sensor.
 * The sampling can be started and stopped by sending the BLOOD_PRESSURE_START_SAMPLING_EVENT
 * and BLOOD_PRESSURE_STOP_SAMPLING_EVENT, respectively. The occurrence of a new sample
 * is signaled by sending the BLOOD_PRESSURE_SAMPLE_EVENT to the subscribed process.
 */
PROCESS(blood_pressure_sensor_process, "Blood pressure sensor process");

PROCESS_THREAD(blood_pressure_sensor_process, event, data)
{
  static struct sensor blood_pressure_sensor = {.sampling_interval = BLOOD_PRESSURE_SAMPLING_INTERVAL*CLOCK_SECOND};
  PROCESS_BEGIN();

  LOG_INFO("Process started.\n");
  BLOOD_PRESSURE_SAMPLE_EVENT = process_alloc_event();
  BLOOD_PRESSURE_START_SAMPLING_EVENT = process_alloc_event();
  BLOOD_PRESSURE_STOP_SAMPLING_EVENT = process_alloc_event();

  while(true) {
    PROCESS_WAIT_EVENT_UNTIL(event == BLOOD_PRESSURE_START_SAMPLING_EVENT);
    blood_pressure_sensor.subscriber = (struct process *)data;
    LOG_INFO("Starting sampling with interval %d s. Subscribed process: %s.\n",
             BLOOD_PRESSURE_SAMPLING_INTERVAL,
             blood_pressure_sensor.subscriber->name);

    /* Initialize the blood pressure value. */
    blood_pressure_sensor.last_sample = prng_rand(BLOOD_PRESSURE_LOWER_BOUND, BLOOD_PRESSURE_UPPER_BOUND);
    etimer_set(&blood_pressure_sensor.sampling_timer, blood_pressure_sensor.sampling_interval);

    while(true) {
      PROCESS_WAIT_EVENT_UNTIL(event == PROCESS_EVENT_TIMER || event == BLOOD_PRESSURE_STOP_SAMPLING_EVENT);

      if(event == PROCESS_EVENT_TIMER) {
        blood_pressure_sensor.last_sample = sensor_generate_sample(blood_pressure_sensor.last_sample,
                                                                   BLOOD_PRESSURE_DEVIATION,
                                                                   BLOOD_PRESSURE_LOWER_BOUND,
                                                                   BLOOD_PRESSURE_UPPER_BOUND);
        LOG_INFO("New sample: %d %s.\n", blood_pressure_sensor.last_sample, BLOOD_PRESSURE_UNIT);
        process_post(blood_pressure_sensor.subscriber, BLOOD_PRESSURE_SAMPLE_EVENT,
                     &blood_pressure_sensor.last_sample);
        etimer_reset(&blood_pressure_sensor.sampling_timer);
        continue;
      }

      if(event == BLOOD_PRESSURE_STOP_SAMPLING_EVENT) {
        LOG_INFO("Stopping sampling.\n");
        etimer_stop(&blood_pressure_sensor.sampling_timer);
        break;
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
