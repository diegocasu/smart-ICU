package it.unipi.smartICU.utils;

import java.util.Map;

/**
 * Interface representing a collector exchanging data
 * with the smart ICU monitors using a generic protocol.
 */
public interface DedicatedCollector {
    /**
     * Gets the monitors registered to this collector.
     * @return  the monitors registered to this collector.
     */
    Map<String, VitalSignsMonitor> getRegisteredMonitors();

    /**
     * Starts the collector.
     */
    void start();

    /**
     * Stops the collector.
     */
    void stop();

    /**
     * Sends a command to turn on the alarm system of a monitor.
     * @param monitorId  the monitor to which the command must be sent.
     */
    void turnOnAlarm(String monitorId);
}
