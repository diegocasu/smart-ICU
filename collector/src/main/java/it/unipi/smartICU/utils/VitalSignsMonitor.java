package it.unipi.smartICU.utils;

/**
 * Class representing a smart ICU monitor registered to the collector.
 */
public class VitalSignsMonitor {
    private final String monitorId;
    private String patientId;
    private boolean alarm;

    public VitalSignsMonitor(String monitorId) {
        this.monitorId = monitorId;
        this.alarm = false;
        this.patientId = "";
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

    public void setPatientId(String patientId) {
        this.patientId = patientId;
    }

    public void setAlarm(boolean alarm) {
        this.alarm = alarm;
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
