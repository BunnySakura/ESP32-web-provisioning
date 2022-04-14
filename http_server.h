#ifndef __http_server_h__
#define __http_server_h__

#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp_http_server.h>

httpd_handle_t http_server_start(void);
void http_server_stop(httpd_handle_t *server);

#endif /* __http_server_h__ */
