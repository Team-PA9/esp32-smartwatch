#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#ifndef WIFI_PROJ_H
#define WIFI_PROJ_H

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

void wifi_init(void);
void task_wifi_init(void *pvParameters);

#endif