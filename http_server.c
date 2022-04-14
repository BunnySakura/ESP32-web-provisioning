#include "http_server.h"
#include "wifi.h"

static const char *TAG = "HTTP_SERVER";

static esp_err_t http_server_get(httpd_req_t *req);
static esp_err_t http_server_post(httpd_req_t *req);
static esp_err_t http_server_404_error(httpd_req_t *req, httpd_err_code_t err);

// 主页
static httpd_uri_t homepage = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = http_server_get,
    .user_ctx = NULL};

// POST设置
static httpd_uri_t post_data = {
    .uri = "/set",
    .method = HTTP_POST,
    .handler = http_server_post,
    .user_ctx = NULL};

httpd_handle_t http_server_start(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &homepage);
        httpd_register_uri_handler(server, &post_data);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_server_404_error);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t http_server_get(httpd_req_t *req)
{
    /* 设置响应头。 */
    httpd_resp_set_hdr(req, "Device", "ESP32");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    FILE *fp = fopen("/spiffs/homepage.html", "r");
    if (fp == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char *html = calloc(4096, sizeof(char));
    fread(html, sizeof(char), 4096, fp);
    ESP_LOGI(TAG, "Length: %d", strlen(html));
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    fclose(fp);
    free(html);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0)
    {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static esp_err_t http_server_post(httpd_req_t *req)
{
    char buf[100] = {0};
    int ret, remaining = req->content_len;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ===========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "=====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);

    // WiFi信息。
    char *wifi_ssid;
    char *wifi_pwd;
    // 以“&”分割字符串并设置指针地址偏移2字符。
    wifi_ssid = strtok(buf, "&");
    wifi_pwd = strtok(NULL, "&");
    // ESP_LOGI(TAG, "SSID:%s PASSWORD:%s", wifi_ssid + 2, wifi_pwd + 2);
    strncpy(wifi_sta_data.ssid, wifi_ssid, strlen(wifi_ssid));
    strncpy(wifi_sta_data.pwd, wifi_pwd, strlen(wifi_pwd));
    ESP_LOGI(TAG, "SSID:%s PASSWORD:%s", wifi_sta_data.ssid + 2, wifi_sta_data.pwd + 2);

    return ESP_OK;
}

static esp_err_t http_server_404_error(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/", req->uri) == 0)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/ URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    else if (strcmp("/set", req->uri) == 0)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/set URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

void http_server_stop(httpd_handle_t *server)
{
    ESP_LOGI(TAG, "Stopping webserver");
    // Stop the httpd server
    httpd_stop(*server);
    server = NULL;
}
