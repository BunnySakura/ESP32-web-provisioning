#ifndef __mqtt_h__
#define __mqtt_h__

#include <string.h>
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

// #define CONFIG_BROKER_URL "mqtt://username:password@host:port"
#define CONFIG_BROKER_URL "mqtt://wwzzdnmsl:wwzzdnmsl@broker.emqx.io:1883"

esp_mqtt_client_handle_t mqtt_app_start(void);
void mqtt_app_stop(esp_mqtt_client_handle_t client);

#endif /* __mqtt_h__ */