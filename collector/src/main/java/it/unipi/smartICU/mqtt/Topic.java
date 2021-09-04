package it.unipi.smartICU.mqtt;

/**
 * Utility methods to manage MQTT topics.
 */
class Topic {
    public static String ALL_PATIENT_STATES_FROM_ALL_MONITORS = "telemetry/smartICU/+/patient-state/+";
    public static String ALL_COMMANDS_TOWARDS_COLLECTOR = "cmd/smartICU/collector/+";
    public static String TURN_ON_ALARM = "cmd/smartICU/%s/patient-state/alarm-state";

    /**
     * Checks if the given topic is a telemetry topic.
     * @param topic  the topic.
     * @return       true if the topic is a telemetry topic, false otherwise.
     */
    public static boolean isTelemetry(String topic) {
        String[] tokens = topic.split("/");
        return tokens[0].equals("telemetry");
    }

    /**
     * Checks if the given topic is a command topic.
     * @param topic  the topic.
     * @return       true if the topic is a command topic, false otherwise.
     */
    public static boolean isCommand(String topic) {
        String[] tokens = topic.split("/");
        return tokens[0].equals("cmd");
    }

    /**
     * Checks if the given topic is a topic for sensor data.
     * @param topic  the topic.
     * @return       true if the topic is a topic for sensor data, false otherwise.
     */
    public static boolean isSample(String topic) {
        String[] tokens = topic.split("/");
        String sensor = tokens[tokens.length - 1];

        return sensor.equals("heart-rate")
                || sensor.equals("blood-pressure")
                || sensor.equals("temperature")
                || sensor.equals("respiration")
                || sensor.equals("oxygen-saturation");
    }

    /**
     * Checks if the given topic is a topic for alarm data.
     * @param topic  the topic.
     * @return       true if the topic is a topic for alarm data, false otherwise.
     */
    public static boolean isAlarmState(String topic) {
        String[] tokens = topic.split("/");
        return tokens[tokens.length - 1].equals("alarm-state");
    }

    /**
     * Checks if the given topic is a topic for patient registration data.
     * @param topic  the topic.
     * @return       true if the topic is a topic for patient registration data, false otherwise.
     */
    public static boolean isPatientRegistration(String topic) {
        String[] tokens = topic.split("/");
        return tokens[tokens.length - 1].equals("patient-registration");
    }

    /**
     * Checks if the given topic is a topic for monitor registration data.
     * @param topic  the topic.
     * @return       true if the topic is a topic for monitor registration data, false otherwise.
     */
    public static boolean isMonitorRegistration(String topic) {
        String[] tokens = topic.split("/");
        return tokens[tokens.length - 1].equals("monitor-registration");
    }

    /**
     * Parses and returns the client ID embedded in the given telemetry topic.
     * @param topic  the topic.
     * @return       the client ID embedded in the telemetry topic.
     */
    public static String getTelemetryClientId(String topic) {
        String[] tokens = topic.split("/");
        return tokens[2];
    }
}
