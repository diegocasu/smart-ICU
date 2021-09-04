package it.unipi.smartICU.utils;

/**
 * Class representing a smart ICU monitor registered to the collector.
 */
public class VitalSignsMonitor {
    private final String monitorId;
    private String patientId;
    private boolean alarm;
    private String ipAddress;
    private int port;

    public VitalSignsMonitor(String monitorId) {
        this.monitorId = monitorId;
        this.alarm = false;
        this.patientId = "";
        this.ipAddress = "";
        this.port = -1;
    }

    public String getMonitorId() {
        return monitorId;
    }

    public String getPatientId() {
        return patientId;
    }

    public boolean getAlarm() {
        return alarm;
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public int getPort() {
        return port;
    }

    public void setPatientId(String patientId) {
        this.patientId = patientId;
    }

    public void setAlarm(boolean alarm) {
        this.alarm = alarm;
    }

    public void setIpAddress(String ipAddress) {
        this.ipAddress = ipAddress;
    }

    public void setPort(int port) {
        this.port = port;
    }

    @Override
    public String toString() {
        return "VitalSignsMonitor{" +
                "monitorId='" + monitorId + '\'' +
                ", patientId='" + patientId + '\'' +
                ", alarm=" + alarm +
                '}';
    }
}
