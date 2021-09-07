CONTIKI_PROJECT = coap-monitor
all: $(CONTIKI_PROJECT)

CONTIKI = ../../../..
SMART_ICU = ../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# Include the CoAP implementation.
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

MODULES_REL += $(SMART_ICU)/vital-signs-monitor/sensors
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/sensors/utils
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/common
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/coap-monitor/utils
MODULES_REL += $(SMART_ICU)/vital-signs-monitor/coap-monitor/resources

include $(CONTIKI)/Makefile.include
