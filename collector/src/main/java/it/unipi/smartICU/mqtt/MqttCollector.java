package it.unipi.smartICU.mqtt;

import com.google.gson.Gson;
import com.google.gson.JsonParseException;

import it.unipi.smartICU.utils.Configuration;
import it.unipi.smartICU.utils.DedicatedCollector;
import it.unipi.smartICU.utils.MessageHandler;
import it.unipi.smartICU.utils.VitalSignsMonitor;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * Class representing an MQTT collector for the smart ICU service.
 * It receives telemetry data from the smart ICU monitors, saving them in a database, and
 * sends commands to monitors to turn on their alarm systems, if requested by the patient
 * health deterioration service.
 */
public class MqttCollector implements MqttCallback, DedicatedCollector {
    private final Logger logger;
    private final Configuration configuration;
    private MqttClient mqttClient;
    private final Map<String, VitalSignsMonitor> registeredMonitors;
    private final String CLIENT_ID = "collector";

    /**
     * Generates the URI of the broker using the available configuration.
     * @return  the URI of the broker.
     */
    private String generateBrokerURI() {
        InetAddress address;

        try {
            address = InetAddress.getByName(configuration.getMqttBrokerIpAddress());
        } catch (UnknownHostException exception) {
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(exception));
            return null;
        }

        if (address instanceof Inet4Address)
            return String.format("tcp://%s:%d",
                                 configuration.getMqttBrokerIpAddress(),
                                 configuration.getMqttBrokerPort());

        if (address instanceof Inet6Address)
            return String.format("tcp://[%s]:%d",
                                 configuration.getMqttBrokerIpAddress(),
                                 configuration.getMqttBrokerPort());

        return null;
    }

    /**
     * Creates a new <code>MqttCollector</code>.
     * @param configuration  the configuration for the collector.
     * @param logger         the logger that will be used by the MQTT collector.
     */
    public MqttCollector(Configuration configuration, Logger logger) {
        this.logger = logger;
        this.configuration = configuration;
        this.registeredMonitors = new HashMap<>();
    }

    @Override
    public Map<String, VitalSignsMonitor> getRegisteredMonitors() {
        return registeredMonitors;
    }

    @Override
    public void stop() {
        try {
            logger.log(Level.INFO, "Stopping the MQTT collector.");
            this.mqttClient.close(true);
        } catch (MqttException mqttException) {
            logger.log(Level.INFO, "An error occurred while stopping the MQTT collector.");
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(mqttException));
        }
    }

    /**
     * Starts the MQTT collector and subscribes to the topics of interest.
     */
    @Override
    public void start() {
        logger.log(Level.INFO, "Starting the MQTT collector.");
        String brokerURI = generateBrokerURI();

        try {
            logger.log(Level.INFO, String.format("Attempting to connect to the broker %s.", brokerURI));
            this.mqttClient = new MqttClient(brokerURI, CLIENT_ID);
            this.mqttClient.setCallback(this);
            this.mqttClient.connect();
            logger.log(Level.INFO, "Connected to the broker.");

            /*
             * Subscribe to all the telemetry topics carrying patient data sent by monitors
             * and to all the command topics carrying instructions for the collector.
             */
            this.mqttClient.subscribe(Topic.ALL_PATIENT_STATES_FROM_ALL_MONITORS);
            this.mqttClient.subscribe(Topic.ALL_COMMANDS_TOWARDS_COLLECTOR);
        } catch (MqttException mqttException) {
            logger.log(Level.INFO, "Failed to connect to the broker.");
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(mqttException));
            stop();
        }
    }

    @Override
    public void turnOnAlarm(String monitorId) {
        String alarmMessage = "{\"alarm\": true}";
        String topic = String.format(Topic.TURN_ON_ALARM, monitorId);

        try {
            logger.log(Level.INFO, String.format("Publishing %s on topic %s.", alarmMessage, topic));
            this.mqttClient.publish(topic, new MqttMessage(alarmMessage.getBytes()));
            registeredMonitors.get(monitorId).setAlarm(true);
        } catch (MqttException mqttException) {
            logger.log(Level.INFO, "Failed to send the message.");
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(mqttException));
        }
    }

    @Override
    public void connectionLost(Throwable throwable) {
        logger.log(Level.INFO, "Lost the connection with the broker.");
        stop();
    }

    @Override
    public void messageArrived(String topic, MqttMessage mqttMessage) {
        String jsonMessage = new String(mqttMessage.getPayload()).trim();
        Map<String, Object> jsonObject = new HashMap<>();

        logger.log(Level.INFO, String.format("New message: [%s], %s.", topic, jsonMessage));

        try {
            jsonObject = (Map<String,Object>) new Gson().fromJson(jsonMessage, jsonObject.getClass());
        } catch (JsonParseException exception) {
            logger.log(Level.INFO,"Discarding the message: JSON parsing error.");
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(exception));
            return;
        }

        if (Topic.isCommand(topic) && Topic.isMonitorRegistration(topic)) {
            MessageHandler.handleMonitorRegistration(logger, registeredMonitors, jsonObject);
            return;
        }

        if (Topic.isCommand(topic) && Topic.isPatientRegistration(topic)) {
            MessageHandler.handlePatientRegistration(logger, registeredMonitors, jsonObject);
            return;
        }

        if (Topic.isTelemetry(topic) && Topic.isAlarmState(topic)) {
            String monitorId = Topic.getTelemetryClientId(topic);
            MessageHandler.handleTelemetryAlarmState(logger, registeredMonitors, monitorId, jsonObject);
            return;
        }

        if (Topic.isTelemetry(topic) && Topic.isSample(topic)) {
            String monitorId = Topic.getTelemetryClientId(topic);
            MessageHandler.handleSample(logger, registeredMonitors, monitorId, jsonObject);
            return;
        }

        logger.log(Level.INFO, "Discarding the message: unknown topic.");
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
        logger.log(Level.INFO, "Message sent successfully.");
    }
}
