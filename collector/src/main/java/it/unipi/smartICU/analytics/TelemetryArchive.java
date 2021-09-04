package it.unipi.smartICU.analytics;

import it.unipi.smartICU.utils.Configuration;
import it.unipi.smartICU.utils.SensorType;

/**
 * Class representing the telemetry data archive for the smart ICU service.
 * It manages the connection to the telemetry database, saving the data
 * received from the smart ICU monitors.
 */
public class TelemetryArchive {

    /**
     * Creates a new <code>TelemetryArchive</code>.
     * @param configuration  the configuration for the telemetry archive.
     */
    public static void init(Configuration configuration) {
        //TODO: store configuration parameters to connect to the database.
    }

    /**
     * Saves a telemetry sample generated by a sensor in the telemetry archive.
     * @param sensor     the type of sensor that produced the sample.
     * @param sample     the sample produced by the sensor.
     * @param unit       the measurement unit of the sample.
     * @param timestamp  the timestamp of the sample.
     */
    public static void save(SensorType sensor, float sample, String unit, float timestamp) {
        //TODO: connect to the database and save the sample.
    }
}