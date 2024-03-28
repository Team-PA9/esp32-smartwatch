#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "time.h"
#include "rtc.h"
#include "RTC/RTC.h"



char Current_Date_Time[100];

static const char *TAG = "wifi station";

// Initialization function to set up RTC and timezone
void init_date_time() {
    // Set timezone to France Standard Time
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

// Function to get the current date/time
void display_date_time(char *date_time) {
    char strftime_buf[64];
    time_t now;
    struct tm timeinfo;

    // Get current time
    time(&now);
    localtime_r(&now, &timeinfo);

    // Format the time into the desired string format
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    
    // Print or log the current date/time
    ESP_LOGI(TAG, "The current date/time in France is: %s", strftime_buf);

    // Copy the formatted time to the provided buffer
    strcpy(date_time, strftime_buf);
}