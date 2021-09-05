package it.unipi.smartICU.utils;

import it.unipi.smartICU.analytics.TelemetryArchive;

import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * Handlers for the messages received from the smart ICU monitors.
 */
public class MessageHandler {

    /**
     * Handles a monitor registration message, adding or removing the monitor
     * to the list of registered ones.
     * @param logger              the logger used to write information about the handling.
     * @param registeredMonitors  the list of registered monitors.
     * @param jsonObject          the parsed JSON message.
     * @return                    the monitor ID if the registration was successful,
     *                            null otherwise.
     */
    public static String handleMonitorRegistration(Logger logger,
                                                   Map<String, VitalSignsMonitor> registeredMonitors,
                                                   Map<String, Object> jsonObject)
    {
        logger.log(Level.INFO, "Handling a monitor registration message.");

        if (jsonObject.containsKey("monitorID") && jsonObject.containsKey("registration")) {
            String monitorId = (String) jsonObject.get("monitorID");
            Boolean registration = (Boolean) jsonObject.get("registration");

            if (!registration) {
                logger.log(Level.INFO, "Removal of the registration is not supported.");
                return null;
            }

            VitalSignsMonitor monitor = new VitalSignsMonitor(monitorId);
            registeredMonitors.put(monitorId, monitor); // If an old object with the same ID is present, it is replaced.
            logger.log(Level.INFO, String.format("Registered a new monitor with ID %s.", monitorId));
            return monitorId;
        }

        logger.log(Level.INFO, "Discarding the message: bad format.");
        return null;
    }

    /**
     * Handles a patient registration message that does not specify the monitor ID
     * in the payload. The patient ID is saved in the registered monitor
     * identified by the passed monitor ID.
     * @param logger              the logger used to write information about the handling.
     * @param registeredMonitors  the list of registered monitors.
     * @param monitorId           the monitor ID of the monitor that sent the message.
     * @param jsonObject          the parsed JSON message.
     */
    public static void handlePatientRegistration(Logger logger,
                                                 Map<String, VitalSignsMonitor> registeredMonitors,
                                                 String monitorId,
                                                 Map<String, Object> jsonObject)
    {
        logger.log(Level.INFO, "Handling a patient registration message.");

        if (jsonObject.containsKey("patientID")) {
            String patientId = (String) jsonObject.get("patientID");
            VitalSignsMonitor monitor = registeredMonitors.get(monitorId);

            if (monitor == null) {
                logger.log(Level.INFO, String.format("Discarding the message: monitor %s is not registered.", monitorId));
                return;
            }

            if (monitor.getPatientId().equals(patientId)) {
                logger.log(Level.INFO, "Discarding the message: the patient ID did not change.");
                return;
            }

            monitor.setPatientId(patientId);
            monitor.setAlarm(false);

            if (monitor.getPatientId().equals(""))
                logger.log(Level.INFO, String.format("Updated monitor %s: removed patient ID.", monitorId));
            else
                logger.log(Level.INFO, String.format("Updated monitor %s: new patient %s.", monitorId, patientId));
            return;
        }

        logger.log(Level.INFO, "Discarding the message: bad format.");
    }

    /**
     * Handles a patient registration message that specifies the monitor ID
     * in the payload. The patient ID is saved in the registered monitor
     * identified by the monitor ID.
     * @param logger              the logger used to write information about the handling.
     * @param registeredMonitors  the list of registered monitors.
     * @param jsonObject          the parsed JSON message.
     */
    public static void handlePatientRegistration(Logger logger,
                                                 Map<String, VitalSignsMonitor> registeredMonitors,
                                                 Map<String, Object> jsonObject)
    {
        if (jsonObject.containsKey("monitorID")) {
            String monitorId = (String) jsonObject.get("monitorID");
            handlePatientRegistration(logger, registeredMonitors, monitorId, jsonObject);
            return;
        }

        logger.log(Level.INFO, "Handling a patient registration message.");
        logger.log(Level.INFO, "Discarding the message: bad format.");
    }

    /**
     * Handles a telemetry message reporting the alarm state of a monitor.
     * It updates the alarm state saved in the registered monitor
     * identified by the passed monitor ID.
     * @param logger              the logger used to write information about the handling.
     * @param registeredMonitors  the list of registered monitors.
     * @param monitorId           the monitor ID of the monitor that sent the message.
     * @param jsonObject          the parsed JSON message.
     */
    public static void handleTelemetryAlarmState(Logger logger,
                                                 Map<String, VitalSignsMonitor> registeredMonitors,
                                                 String monitorId,
                                                 Map<String, Object> jsonObject)
    {
        logger.log(Level.INFO, "Handling a telemetry alarm state message.");

        if (jsonObject.containsKey("alarm")) {
            Boolean alarm = (Boolean) jsonObject.get("alarm");
            VitalSignsMonitor monitor = registeredMonitors.get(monitorId);

            if (monitor == null) {
                logger.log(Level.INFO, String.format("Discarding the message: monitor %s is not registered.", monitorId));
                return;
            }

            if (monitor.getAlarm() == alarm) {
                logger.log(Level.INFO, "Discarding the message: the alarm state did not change.");
                return;
            }

            monitor.setAlarm(alarm);
            logger.log(Level.INFO, String.format("Updated monitor %s: new alarm state \"%s\".", monitorId, alarm));
            return;
        }

        logger.log(Level.INFO, "Discarding the message: bad format.");
    }

    /**
     * Handles a telemetry message carrying a sample produced by a sensor,
     * saving it inside the telemetry database.
     * @param logger              the logger used to write information about the handling.
     * @param registeredMonitors  the list of registered monitors.
     * @param monitorId           the monitor ID of the monitor that sent the message.
     * @param jsonObject          the parsed JSON message.
     */
    public static void handleSample(Logger logger,
                                    Map<String, VitalSignsMonitor> registeredMonitors,
                                    String monitorId,
                                    Map<String, Object> jsonObject)
    {
        logger.log(Level.INFO, "Handling a sensor sample message.");

        if (registeredMonitors.get(monitorId) == null) {
            logger.log(Level.INFO, String.format("Discarding the message: monitor %s is not registered.", monitorId));
            return;
        }

        if (jsonObject.containsKey("unit") && jsonObject.containsKey("timestamp")) {
            SensorType sensor = null;
            float sample = -1f;

            if (jsonObject.containsKey("heartRate")) {
                sensor = SensorType.HEART_RATE;
                sample = Float.parseFloat((jsonObject.get("heartRate").toString()));

            } else if (jsonObject.containsKey("bloodPressure")) {
                sensor = SensorType.BLOOD_PRESSURE;
                sample = Float.parseFloat((jsonObject.get("bloodPressure").toString()));

            } else if (jsonObject.containsKey("temperature")) {
                sensor = SensorType.TEMPERATURE;
                sample = Float.parseFloat((jsonObject.get("temperature").toString()));

            } else if (jsonObject.containsKey("respiration")) {
                sensor = SensorType.RESPIRATION;
                sample = Float.parseFloat((jsonObject.get("respiration").toString()));

            } else if (jsonObject.containsKey("oxygenSaturation")) {
                sensor = SensorType.OXYGEN_SATURATION;
                sample = Float.parseFloat((jsonObject.get("oxygenSaturation").toString()));
            }

            if (sensor != null) {
                String unit = (String) jsonObject.get("unit");
                float timestamp = Float.parseFloat(jsonObject.get("timestamp").toString());
                TelemetryArchive.save(sensor, sample, unit, timestamp, monitorId,
                                      registeredMonitors.get(monitorId).getPatientId());
                return;
            }
        }

        logger.log(Level.INFO, "Discarding the message: bad format.");
    }
}
