#include "spiffs.h"
#include "wifi.h"

static const char *TAG = "SPIFFS";

void spiffs_init(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
}

esp_err_t read_wifi_configuration(void)
{
    FILE *fp = fopen("/spiffs/wifi.conf", "rb");
    if (fp == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    fread(wifi_sta_data.ssid, sizeof(char), 32, fp);
    fread(wifi_sta_data.pwd, sizeof(char), 64, fp);
    ESP_LOGI(TAG, "%s\t%s", wifi_sta_data.ssid, wifi_sta_data.pwd);
    fclose(fp);
    return ESP_OK;
}

esp_err_t write_wifi_configuration(void)
{
    FILE *fp = fopen("/spiffs/wifi.conf", "wb");
    if (fp == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    fwrite(wifi_sta_data.ssid, sizeof(char), 32, fp);
    fwrite(wifi_sta_data.pwd, sizeof(char), 64, fp);
    ESP_LOGI(TAG, "%s\t%s", wifi_sta_data.ssid, wifi_sta_data.pwd);
    fclose(fp);
    return ESP_OK;
}