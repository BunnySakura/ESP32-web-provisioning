#ifndef __wifi_h__
#define __wifi_h__

#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define AP_ESP_WIFI_SSID "Boy Next Door"
#define AP_ESP_WIFI_PASS "12345678"
#define AP_ESP_WIFI_CHANNEL 6
#define AP_MAX_STA_CONN 4

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

// 保存WiFi的SSID和密码的结构体。
typedef struct wifi_configuration
{
    char ssid[32];
    char pwd[64];
} wifi_configuration_t;
wifi_configuration_t wifi_sta_data;

// WiFi在station模式连接状态标志。
esp_err_t wifi_connect_flag;

void wifi_init(void);

void wifi_init_softap(void);
void wifi_stop_softap(void);

void wifi_init_sta(const char STA_ESP_WIFI_SSID[32], const char STA_ESP_WIFI_PASS[64]);
void wifi_stop_sta(void);

#endif /* __wifi_h__ */
