CONTIKI_PROJECT = mqtt-monitor
all: $(CONTIKI_PROJECT)

CONTIKI = ../../../..
SMART_ICU = ../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# Include the MQTT implementation.
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/mqtt

MODULES_REL += $(SMART_ICU)/vital-signs-monitor/sensors
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/sensors/utils
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/mqtt-monitor/utils
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/common

include $(CONTIKI)/Makefile.include
