#include "spiffs.h"
#include "wifi.h"
#include "http_server.h"
#include "aht10.h"
#include "mqtt.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    spiffs_init();
    wifi_init();

    wifi_connect_flag = ESP_FAIL;
    memset(&wifi_sta_data, 0, 96);

    httpd_handle_t http_server;

    if (read_wifi_configuration() == ESP_OK && strncmp(wifi_sta_data.ssid, "", 32))
    {
        // 如果读取文件成功且读取数据非空，则尝试连接AP。
        wifi_init_sta(wifi_sta_data.ssid + 2, wifi_sta_data.pwd + 2);
        if (wifi_connect_flag == ESP_FAIL)
        {
            wifi_stop_sta();

            // 进入配网。
            wifi_init_softap();
            http_server = http_server_start();
        }
    }
    else
    {
        // 进入配网。
        wifi_init_softap();
        http_server = http_server_start();
    }

    while (wifi_connect_flag == ESP_FAIL)
    {
        if (strncmp(wifi_sta_data.ssid, "", 32))
        {
            // 终止AP模式和http服务。
            wifi_stop_softap();
            http_server_stop(&http_server);
            // 如果数据非空，则尝试连接AP。
            wifi_init_sta(wifi_sta_data.ssid + 2, wifi_sta_data.pwd + 2);
            if (wifi_connect_flag == ESP_OK)
            {
                write_wifi_configuration();
            }
            else
            {
                wifi_stop_sta();

                // 进入配网。
                memset(&wifi_sta_data, 0, 96);
                wifi_init_softap();
                http_server = http_server_start();
            }
        }
        else
        {
            ESP_LOGI(TAG, "wait to connect to wifi AP");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    // AHT10初始化、MQTT协议初始化。
    aht10_init();
    esp_mqtt_client_handle_t mqtt_client = mqtt_app_start();

    float temp;
    float humi;
    // Initialise the xLastWakeTime variable with the current time.
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        temp = read_temperature();
        humi = read_humidity();

        char publish_str[64];
        sprintf(publish_str, "%.2f\t%.2f", temp, humi);
        esp_mqtt_client_publish(mqtt_client, "topic/sensor", publish_str, 0, 1, 0);

        ESP_LOGI(TAG, "temp:%f\thumi:%f", temp, humi);

        // Wait for the next cycle.
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3000));
    }
}