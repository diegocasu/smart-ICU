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

    public int getPatientHealthDeteriorationSchedulingRate() {
        return patientHealthDeteriorationSchedulingRate;
    }

    @Override
    public String toString() {
        return new GsonBuilder().setPrettyPrinting().create().toJson(this);
    }
}
