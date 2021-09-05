package it.unipi.smartICU.utils;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonParseException;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;


/**
 * Class representing a JSON configuration file.
 * It stores the information needed to execute the collector.
 */
public class Configuration {
    private final String filePath;
    private String mqttBrokerIpAddress;
    private int mqttBrokerPort;
    private String coapCollectorIpAddress;
    private int coapCollectorPort;
    private String telemetryArchiveIpAddress;
    private int telemetryArchivePort;
    private String telemetryArchiveUser;
    private String telemetryArchivePassword;
    private String telemetryArchiveDatabaseName;
    private int patientHealthDeteriorationSchedulingRate;

    /**
     * Parses the JSON configuration file.
     * @throws IOException         if an I/O error occurs while accessing the file.
     * @throws JsonParseException  if the file contains an invalid JSON syntax.
     */
    private void parseConfiguration() throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(filePath));
        Configuration parsedConfiguration = new Gson().fromJson(reader, Configuration.class);

        this.mqttBrokerIpAddress = parsedConfiguration.mqttBrokerIpAddress;
        this.mqttBrokerPort = parsedConfiguration.mqttBrokerPort;
        this.coapCollectorIpAddress = parsedConfiguration.coapCollectorIpAddress;
        this.coapCollectorPort = parsedConfiguration.coapCollectorPort;
        this.telemetryArchiveIpAddress = parsedConfiguration.telemetryArchiveIpAddress;
        this.telemetryArchivePort = parsedConfiguration.telemetryArchivePort;
        this.telemetryArchiveUser = parsedConfiguration.telemetryArchiveUser;
        this.telemetryArchivePassword = parsedConfiguration.telemetryArchivePassword;
        this.telemetryArchiveDatabaseName = parsedConfiguration.telemetryArchiveDatabaseName;
        this.patientHealthDeteriorationSchedulingRate = parsedConfiguration.patientHealthDeteriorationSchedulingRate;

        reader.close();
    }

    /**
     * Parses a JSON configuration file and creates a corresponding Configuration object.
     * @param filePath             the path of the configuration file.
     * @throws IOException         if an I/O error occurs while accessing the file.
     * @throws JsonParseException  if the file contains an invalid JSON syntax.
     */
    public Configuration(String filePath) throws IOException {
        this.filePath = filePath;
        parseConfiguration();
    }

    public String getFilePath() {
        return filePath;
    }

    public String getMqttBrokerIpAddress() {
        return mqttBrokerIpAddress;
    }

    public int getMqttBrokerPort() {
        return mqttBrokerPort;
    }

    public String getCoapCollectorIpAddress() {
        return coapCollectorIpAddress;
    }

    public int getCoapCollectorPort() {
        return coapCollectorPort;
    }

    public String getTelemetryArchiveIpAddress() {
        return telemetryArchiveIpAddress;
    }

    public int getTelemetryArchivePort() {
        return telemetryArchivePort;
    }

    public String getTelemetryArchiveUser() {
        return telemetryArchiveUser;
    }

    public String getTelemetryArchivePassword() {
        return telemetryArchivePassword;
    }

    public String getTelemetryArchiveDatabaseName() {
        return telemetryArchiveDatabaseName;
    }

    public int getPatientHealthDeteriorationSchedulingRate() {
        return patientHealthDeteriorationSchedulingRate;
    }

    @Override
    public String toString() {
        return new GsonBuilder().setPrettyPrinting().create().toJson(this);
    }
}
