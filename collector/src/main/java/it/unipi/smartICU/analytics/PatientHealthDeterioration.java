package it.unipi.smartICU.analytics;

import it.unipi.smartICU.coap.CoapCollector;
import it.unipi.smartICU.mqtt.MqttCollector;
import it.unipi.smartICU.utils.DedicatedCollector;
import it.unipi.smartICU.utils.VitalSignsMonitor;

import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Class implementing the patient health deterioration service.
 * In reality, this service should read the telemetry data produced
 * by patients currently attached to monitors and produce an early warning
 * score for each of them, based on which the alarm system would be turned
 * on or not.<br>
 * For simplicity, currently the behaviour is simulated by
 * randomly deciding if the patient health is deteriorating or not.
 */
public class PatientHealthDeterioration implements Runnable {
    private final Logger logger;
    private final MqttCollector mqttCollector;
    private final CoapCollector coapCollector;
    private final Random rng;

    /**
     * Checks if the health of patients is deteriorating. The check involves
     * the patients attached to the monitors registered to the given collector.
     * @param collector  the collector holding the list of monitors.
     */
    private void healthDeteriorationCheck(DedicatedCollector collector ) {
        for (String monitorId : collector.getRegisteredMonitors().keySet()) {
            VitalSignsMonitor monitor = collector.getRegisteredMonitors().get(monitorId);

            // If the monitor has already the alarm turned on or has not a patient attached, it is skipped.
            if (monitor.getAlarm() || monitor.getPatientId().equals(""))
                continue;

            // Extract a random boolean to simulate a patient health deterioration condition.
            boolean patientHealthDeterioration = rng.nextBoolean();

            if (patientHealthDeterioration) {
                logger.log(Level.INFO, String.format("Detected a health deterioration of patient %s attached to monitor %s.",
                                                     monitor.getPatientId(),
                                                     monitorId));
                collector.turnOnAlarm(monitorId);
            }
        }
    }

    public PatientHealthDeterioration(Logger logger, MqttCollector mqttCollector, CoapCollector coapCollector) {
        this.logger = logger;
        this.mqttCollector = mqttCollector;
        this.coapCollector = coapCollector;
        this.rng = new Random("smartICU".hashCode());
    }

    @Override
    public void run() {
        logger.log(Level.INFO, "Starting the patient health deterioration check.");
        healthDeteriorationCheck(mqttCollector);
        healthDeteriorationCheck(coapCollector);
        logger.log(Level.INFO, "Ending the patient health deterioration check.");
    }
}
