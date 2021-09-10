package it.unipi.smartICU;

import com.google.gson.JsonParseException;

import it.unipi.smartICU.analytics.TelemetryArchive;
import it.unipi.smartICU.coap.CoapCollector;
import it.unipi.smartICU.mqtt.MqttCollector;
import it.unipi.smartICU.utils.Configuration;
import it.unipi.smartICU.analytics.PatientHealthDeterioration;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.lang3.exception.ExceptionUtils;

import java.io.IOException;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;
import java.util.concurrent.TimeUnit;


/**
 * Class representing a collector for the smart ICU service.
 * It instantiates an MQTT collector and a CoAP collector to receive telemetry data
 * from the smart ICU monitors. The received samples are stored in a database and
 * periodically analyzed to detect patients whose conditions are deteriorating: if
 * a patient whose health is worsening is found, the relative monitor receives
 * a command to turn on its alarm system.
 */
public class Collector {
    private final ScheduledExecutorService scheduler;
    private Configuration configuration;
    private Logger logger;
    private final static String LOGGER_NAME = "it.unipi.smartICU";
    private final static String LOGGER_PROPERTIES = "/logging.properties";

    /**
     * Loads the configuration of the logger stored in <code>resources/LOGGER_PROPERTIES</code>.
     * A failure in the loading of the properties does not prevent the execution of the collector.
     */
    private void loadLoggerConfiguration() {
        try {
            LogManager.getLogManager().readConfiguration(Collector.class.getResourceAsStream(LOGGER_PROPERTIES));
        } catch (IOException exception) {
            logger.log(Level.INFO, "Failed to load the logger configuration.");
            logger.log(Level.FINE, ExceptionUtils.getStackTrace(exception));
        }
    }

    /**
     * Creates a new <code>Collector</code>.
     * @param configuration  the configuration for the collector.
     */
    public Collector(Configuration configuration) {
        this.scheduler = Executors.newScheduledThreadPool(1);
        this.logger = Logger.getLogger(LOGGER_NAME);
        this.configuration = configuration;
        loadLoggerConfiguration();
    }

    /**
     * Starts the collector initializing the telemetry database,
     * instantiating the MQTT and CoAP collectors and starting
     * the periodic service to check the patient health deterioration.
     */
    public void start() {
        logger.log(Level.INFO, "Initializing the connection to the telemetry database.");
        if (!TelemetryArchive.init(configuration, logger))
            return;

        logger.log(Level.INFO, "Instantiating the MQTT collector.");
        MqttCollector mqttCollector = new MqttCollector(configuration, logger);
        mqttCollector.start();

        logger.log(Level.INFO, "Instantiating the CoAP collector.");
        CoapCollector coapCollector = new CoapCollector(configuration, logger);
        coapCollector.start();

        logger.log(Level.INFO, "Scheduling the execution of the patient health deterioration routine.");
        PatientHealthDeterioration healthDeteriorationTask = new PatientHealthDeterioration(logger,
                                                                                            mqttCollector,
                                                                                            coapCollector);
        scheduler.scheduleAtFixedRate(healthDeteriorationTask,
                                      configuration.getPatientHealthDeteriorationSchedulingRate(),
                                      configuration.getPatientHealthDeteriorationSchedulingRate(),
                                      TimeUnit.SECONDS);
    }

    /**
     * Starts the execution of the collector.
     * The path of the configuration file must be specified as argument when
     * invoking the <code>main()</code> method from the command line.
     * @throws IOException         if an I/O error occurs while accessing the configuration file.
     * @throws JsonParseException  if the configuration file contains an invalid JSON syntax.
     */
    public static void main(String[] args) throws IOException {
        CommandLineParser parser = new DefaultParser();
        HelpFormatter formatter = new HelpFormatter();
        CommandLine cmd;
        Options options = new Options();

        Option configurationOption = new Option("c", "configuration", true, "Configuration file path");
        configurationOption.setRequired(true);
        options.addOption(configurationOption);

        try {
            cmd = parser.parse(options, args);
        } catch (ParseException exception) {
            System.out.println(exception.getMessage());
            formatter.printHelp("smartICU collector", options);
            return;
        }

        Configuration configuration = new Configuration(cmd.getOptionValue("configuration"));
        Collector collector = new Collector(configuration);
        collector.start();
    }
}
