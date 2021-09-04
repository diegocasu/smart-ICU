package it.unipi.smartICU.coap.resources;

import com.google.gson.Gson;
import com.google.gson.JsonParseException;

import it.unipi.smartICU.coap.CoapCollector;
import it.unipi.smartICU.utils.MessageHandler;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.coap.Response;
import org.eclipse.californium.core.server.resources.CoapExchange;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.StringJoiner;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * Class representing a CoAP resource holding the registered monitors
 * of the CoAP collector.
 */
public class RegisteredMonitorsResource extends CoapResource {
    private final Logger logger;
    private final CoapCollector coapCollector;

    /**
     * Parses and returns the JSON object contained in the given string.
     * @param json  the string in JSON format.
     * @return      the JSON object if the string is correctly formatted in JSON,
     *              null otherwise.
     */
    private Map<String, Object> parseJson(String json) {
        Map<String, Object> jsonObject = new HashMap<>();

        try {
            jsonObject = (Map<String,Object>) new Gson().fromJson(json, jsonObject.getClass());
        } catch (JsonParseException exception) {
            logger.log(Level.INFO,"Discarding the message: JSON parsing error.");
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(exception));
            return null;
        }

        return jsonObject;
    }

    /**
     * Handles the occurrence of an error when observing a resource.
     * @param coapClient  the CoAP client that is observing the resource.
     * @param serverURI   the URI of the observed resource.
     */
    private void onObserverRelationError(CoapClient coapClient, String serverURI) {
        logger.log(Level.INFO,
                   String.format("An error occurred while observing %s. Closing the observe relation.", serverURI));
        coapClient.shutdown();
    }

    /**
     * Establishes an observe relation for the registered patient resource held by a monitor.
     * @param exchange   the POST registration request issued by the monitor.
     * @param monitorId  the ID of the monitor.
     */
    private void setupRegisteredPatientObserveRelation(CoapExchange exchange, String monitorId) {
        CoapClient coapClient = new CoapClient(String.format("coap://[%s]:%s/registeredPatient",
                                                             exchange.getSourceAddress().getHostAddress(),
                                                             exchange.getSourcePort()));
        coapClient.observe(new CoapHandler() {
            @Override
            public void onLoad(CoapResponse coapResponse) {
                logger.log(Level.INFO, String.format("New observer GET of %s, with payload %s.",
                                                     coapClient.getURI(),
                                                     coapResponse.getResponseText().trim()));

                Map<String, Object> jsonObject = parseJson(coapResponse.getResponseText().trim());
                if (jsonObject == null)
                    return;

                MessageHandler.handlePatientRegistration(logger,
                                                         coapCollector.getRegisteredMonitors(),
                                                         monitorId,
                                                         jsonObject);
            }

            @Override
            public void onError() {
                onObserverRelationError(coapClient, coapClient.getURI());
            }
        });
    }

    /**
     * Establishes an observe relation for the alarm state resource held by a monitor.
     * @param exchange   the POST registration request issued by the monitor.
     * @param monitorId  the ID of the monitor.
     */
    private void setupAlarmStateObserveRelation(CoapExchange exchange, String monitorId) {
        CoapClient coapClient = new CoapClient(String.format("coap://[%s]:%s/patientState/alarmState",
                                                             exchange.getSourceAddress().getHostAddress(),
                                                             exchange.getSourcePort()));
        coapClient.observe(new CoapHandler() {
            @Override
            public void onLoad(CoapResponse coapResponse) {
                logger.log(Level.INFO, String.format("New observer GET of %s, with payload %s.",
                                                     coapClient.getURI(),
                                                     coapResponse.getResponseText().trim()));

                Map<String, Object> jsonObject = parseJson(coapResponse.getResponseText().trim());
                if (jsonObject == null)
                    return;

                MessageHandler.handleTelemetryAlarmState(logger,
                                                         coapCollector.getRegisteredMonitors(),
                                                         monitorId,
                                                         jsonObject);
            }

            @Override
            public void onError() {
                onObserverRelationError(coapClient, coapClient.getURI());
            }
        });

    }

    /**
     * Establishes an observe relation for all the resources exposing telemetry data
     * sampled by sensors held by a monitor
     * @param exchange   the POST registration request issued by the monitor.
     * @param monitorId  the ID of the monitor.
     */
    private void setupSensorsObserveRelation(CoapExchange exchange, String monitorId) {
        List<CoapClient> coapClients = Arrays.asList(
                new CoapClient(String.format("coap://[%s]:%s/patientState/heartRate", exchange.getSourceAddress().getHostAddress(), exchange.getSourcePort())),
                new CoapClient(String.format("coap://[%s]:%s/patientState/bloodPressure", exchange.getSourceAddress().getHostAddress(), exchange.getSourcePort())),
                new CoapClient(String.format("coap://[%s]:%s/patientState/temperature", exchange.getSourceAddress().getHostAddress(), exchange.getSourcePort())),
                new CoapClient(String.format("coap://[%s]:%s/patientState/respiration", exchange.getSourceAddress().getHostAddress(), exchange.getSourcePort())),
                new CoapClient(String.format("coap://[%s]:%s/patientState/oxygenSaturation", exchange.getSourceAddress().getHostAddress(), exchange.getSourcePort()))
        );

        for (CoapClient coapClient : coapClients) {
            coapClient.observe(new CoapHandler() {
                @Override
                public void onLoad(CoapResponse coapResponse) {
                    logger.log(Level.INFO, String.format("New observer GET of %s, with payload %s.",
                                                         coapClient.getURI(),
                                                         coapResponse.getResponseText().trim()));

                    Map<String, Object> jsonObject = parseJson(coapResponse.getResponseText().trim());
                    if (jsonObject == null)
                        return;

                    MessageHandler.handleSample(logger,
                                                coapCollector.getRegisteredMonitors(),
                                                monitorId,
                                                jsonObject);
                }

                @Override
                public void onError() {
                    onObserverRelationError(coapClient, coapClient.getURI());
                }
            });
        }
    }

    /**
     * Creates a new <code>RegisteredMonitorsResource</code>.
     * @param name           the name with which the resource will be identified.
     * @param coapCollector  the CoAP collector that hosts the resource.
     * @param logger         the logger that will be used by the resource.
     */
    public RegisteredMonitorsResource(String name, CoapCollector coapCollector, Logger logger) {
        super(name);
        this.logger = logger;
        this.coapCollector = coapCollector;
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        Response response = new Response(CoAP.ResponseCode.CONTENT);
        response.getOptions().setContentFormat(MediaTypeRegistry.APPLICATION_JSON);

        StringJoiner payload = new StringJoiner(",", "{\"registeredMonitors\": [", "]}");
        for (String monitorId : coapCollector.getRegisteredMonitors().keySet())
            payload.add("\"" + monitorId + "\"");

        response.setPayload(payload.toString());
        exchange.respond(response);
    }

    @Override
    public void handlePOST(CoapExchange exchange) {
        logger.log(Level.INFO,
                   String.format("New POST from %s, with payload %s.",
                                 exchange.getSourceAddress().getHostAddress(),
                                 exchange.getRequestText().trim()));

        Map<String, Object> jsonObject = parseJson(exchange.getRequestText().trim());
        if (jsonObject == null) {
            exchange.respond(CoAP.ResponseCode.BAD_REQUEST);
            return;
        }

        String monitorID = MessageHandler.handleMonitorRegistration(logger,
                                                                    coapCollector.getRegisteredMonitors(),
                                                                    jsonObject);
        if (monitorID == null) {
            exchange.respond(CoAP.ResponseCode.BAD_REQUEST);
            return;
        }

        exchange.respond(CoAP.ResponseCode.CREATED);
        coapCollector.getRegisteredMonitors().get(monitorID).setIpAddress(exchange.getSourceAddress().getHostAddress());
        coapCollector.getRegisteredMonitors().get(monitorID).setPort(exchange.getSourcePort());

        logger.log(Level.INFO, String.format("Establishing the observer relations with %s.", exchange.getSourceAddress().getHostAddress()));
        setupRegisteredPatientObserveRelation(exchange, monitorID);
        setupAlarmStateObserveRelation(exchange, monitorID);
        setupSensorsObserveRelation(exchange, monitorID);
    }
}
