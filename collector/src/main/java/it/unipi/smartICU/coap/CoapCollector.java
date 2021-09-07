package it.unipi.smartICU.coap;

import it.unipi.smartICU.coap.resources.RegisteredMonitorsResource;
import it.unipi.smartICU.utils.Configuration;

import it.unipi.smartICU.utils.DedicatedCollector;
import it.unipi.smartICU.utils.VitalSignsMonitor;
import org.apache.commons.lang3.exception.ExceptionUtils;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapServer;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.core.network.CoapEndpoint;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * Class representing a CoAP collector for the smart ICU service.
 * It receives telemetry data from the smart ICU monitors, saving them in a database, and
 * sends commands to monitors to turn on their alarm systems, if requested by the patient
 * health deterioration service.
 */
public class CoapCollector extends CoapServer implements DedicatedCollector {
    private final Logger logger;
    private final Configuration configuration;
    private final Map<String, VitalSignsMonitor> registeredMonitors;
    private final CoapClient alarmCommandsClient;

    /**
     * Creates a new <code>CoapCollector</code>.
     * @param configuration  the configuration for the collector.
     * @param logger         the logger that will be used by the CoAP collector.
     */
    public CoapCollector(Configuration configuration, Logger logger) {
        super();
        this.configuration = configuration;
        this.logger = logger;
        this.registeredMonitors = new HashMap<>();
        this.alarmCommandsClient = new CoapClient();
    }

    @Override
    public Map<String, VitalSignsMonitor> getRegisteredMonitors() {
        return registeredMonitors;
    }

    @Override
    public void stop() {
        logger.log(Level.INFO, "Stopping the CoAP collector.");
        super.stop();
    }

    @Override
    public void start() {
        logger.log(Level.INFO, "Starting the CoAP collector.");
        addEndpoint(new CoapEndpoint(new InetSocketAddress(configuration.getCoapCollectorIpAddress(),
                                                           configuration.getCoapCollectorPort())));
        add(new RegisteredMonitorsResource("registeredMonitors", this, logger));
        super.start();
    }

    @Override
    public void turnOnAlarm(String monitorId) {
        String alarmMessage = "{\"alarm\": true}";
        InetAddress address;

        try {
            address = InetAddress.getByName(registeredMonitors.get(monitorId).getIpAddress());
        } catch (UnknownHostException exception) {
            logger.log(Level.INFO, String.format("Impossible to issue the PUT: the address %s of the monitor is malformed.",
                                                 registeredMonitors.get(monitorId).getIpAddress()));
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(exception));
            return;
        }

        if (address instanceof Inet4Address)
            alarmCommandsClient.setURI(String.format("coap://%s:%s/patientState/alarmState",
                                                     registeredMonitors.get(monitorId).getIpAddress(),
                                                     registeredMonitors.get(monitorId).getPort()));
        else if (address instanceof Inet6Address)
            alarmCommandsClient.setURI(String.format("coap://[%s]:%s/patientState/alarmState",
                                                     registeredMonitors.get(monitorId).getIpAddress(),
                                                     registeredMonitors.get(monitorId).getPort()));

        logger.log(Level.INFO, String.format("Issuing a PUT to %s, with payload %s.", alarmCommandsClient.getURI(), alarmMessage));
        alarmCommandsClient.put(alarmMessage, MediaTypeRegistry.APPLICATION_JSON);;
        registeredMonitors.get(monitorId).setAlarm(true);
    }
}
