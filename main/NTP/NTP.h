#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "time.h"
#include "rtc.h"

#include "esp_sntp.h"

void time_sync_notification_cb(struct timeval *tv);
static void initialize_sntp(void);
static void obtain_time(void);
void Set_SystemTime_SNTP();
