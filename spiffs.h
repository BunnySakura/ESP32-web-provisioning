#ifndef __spiffs_h__
#define __spiffs_h__

#include "esp_spiffs.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"

void spiffs_init(void);
esp_err_t read_wifi_configuration(void);
esp_err_t write_wifi_configuration(void);

#endif /* __spiffs_h__ */
