# smart-ICU

Project for the Internet of Things course of the Master of Science 
in Computer Engineering, University of Pisa.

The project consists in the design and implementation of an IoT telemetry and control system for 
vital signs monitors deployed in an intensive care unit, with the capability of detecting and 
signalling early deterioration of patients' health.

## Overview
The repository is organised in the following way:
- _simulations_ contains simulation scenarios to be imported in Cooja.
- _border-router_ contains the source code of the Contiki-NG border router.
- _collector_ contains the source code of the Java collector.
- _vital-signs-monitor_ contains the source code of both MQTT and CoAP-based monitors,
  together with the modules simulating their sensors and alarm systems.

## Requirements
- The [Contiki-NG](https://github.com/contiki-ng/contiki-ng) OS, together with the Cooja simulator.
- Java 8 and Maven installed in the node that will run the collector.
- An MQTT broker (for example ```mosquitto```).
- A MySQL database.

## Installation, configuration and execution
The following steps allow to execute an example simulation with all the components deployed
in a single machine:
- Move the entire folder containing this project into the ```contiki-ng/examples``` folder.
- Change the log levels and eventually enable the automatic configuration of the patient ID 
  by modifying ```vital-signs-monitor/coap-monitor/project-conf.h``` and 
  ```vital-signs-monitor/mqtt-monitor/project-conf.h```.
- Inside the ```collector``` folder, compile the collector with the command:
  ```bash
  mvn clean install
  ```
- Run the MQTT broker.
- Run MySQL and create a schema dedicated to this project, for example called ```smartICU```.
- Prepare a JSON configuration file for the collector, for example called ```configuration.json```,
  with the following structure:
  ```
  {
    "mqttBrokerIpAddress": "localhost",
    "mqttBrokerPort": 1883,
    "coapCollectorIpAddress": "fd00::1",
    "coapCollectorPort": 5683,
    "telemetryArchiveIpAddress": "localhost",
    "telemetryArchivePort": 3306,
    "telemetryArchiveUser": "yourUser",
    "telemetryArchivePassword": "yourPassword",
    "telemetryArchiveDatabaseName": "yourSchema",
    "patientHealthDeteriorationSchedulingRate": 60
  }
  ```
  where ```patientHealthDeteriorationSchedulingRate``` is expressed in seconds.  
  If the ports used by the MQTT broker and the CoAP collector are not 1883 and 5683 respectively,
  change them accordingly in the files ```vital-signs-monitor/mqtt-monitor/utils/mqtt-monitor-constants.h```
  and ```vital-signs-monitor/coap-monitor/utils/coap-monitor-constants.h```.
- Open Cooja and import the simulation ```simulations/example.csc```, which is a simple scenario with
  one border router, one MQTT monitor and one CoAP monitor.
- Again in Cooja, configure the border router (node 1) to use tunslip6 by adding and starting a serial socket:
  ```bash
  Tools > Serial Socket (SERVER) > Contiki 1
  Start
  ```
  Then, inside the ```border-router``` folder, execute:
  ```bash
  make TARGET=cooja connect-router-cooja
  ```
- Inside the ```collector``` folder, start the collector with the command:
  ```bash
  java -jar target/smartICU-collector-1.0-SNAPSHOT-jar-with-dependencies.jar -c configuration.json
  ```
  The messages printed on the screen should confirm that both the MQTT and CoAP collectors 
  are correctly running.
- Start the simulation in Cooja, providing the patient ID via the serial interface when requested.
  More details about the interaction with the nodes and the communication flow can be found
  in the documentation.

## Modify the behaviour of nodes
The parameters of nodes, included the sampling rate of sensors, can be modified in the following files:
- ```vital-signs-monitor/mqtt-monitor/project-conf.h```
- ```vital-signs-monitor/mqtt-monitor/utils/mqtt-monitor-constants.h```
- ```vital-signs-monitor/coap-monitor/project-conf.h```
- ```vital-signs-monitor/coap-monitor/utils/coap-monitor-constants.h```
- ```vital-signs-monitor/common/alarm-constants.h```
- ```vital-signs-monitor/common/sensors/utils/sensor-constants.h```