#ifndef __PROJECT_CONF_H
#define __PROJECT_CONF_H

/* Log levels. */
#define LOG_LEVEL_HEART_RATE_SENSOR          LOG_LEVEL_INFO
#define LOG_LEVEL_BLOOD_PRESSURE_SENSOR      LOG_LEVEL_INFO
#define LOG_LEVEL_TEMPERATURE_SENSOR         LOG_LEVEL_INFO
#define LOG_LEVEL_RESPIRATION_SENSOR         LOG_LEVEL_INFO
#define LOG_LEVEL_OXYGEN_SATURATION_SENSOR   LOG_LEVEL_INFO
#define LOG_LEVEL_ALARM_SYSTEM               LOG_LEVEL_INFO
#define LOG_LEVEL_COAP_MONITOR               LOG_LEVEL_DBG
#define LOG_LEVEL_COAP_RESOURCES             LOG_LEVEL_DBG

/* PAN ID configuration. */
#undef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID 0x0041

/* Set the max CoAP response payload before enabling fragmentation. */
#undef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE 64

/* Set the maximum number of CoAP concurrent transactions. */
#undef COAP_MAX_OPEN_TRANSACTIONS
#define COAP_MAX_OPEN_TRANSACTIONS 7

/* Set the maximum number of CoAP observers. */
#undef COAP_MAX_OBSERVERS
#define COAP_MAX_OBSERVERS 7

/* IPv6 parameters to reduce OS size. */
#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 10
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 10
#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE 240

#endif /* __PROJECT_CONF_H */
