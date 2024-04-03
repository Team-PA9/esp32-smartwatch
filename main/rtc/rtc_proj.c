#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "time.h"
#include "rtc.h"
#include "rtc/rtc_proj.h"

static const char *TAG = "wifi station";

// Initialization function to set up RTC and timezone
void tz_init()
 {
    // Set timezone to France Standard Time
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

// Function to get the current date/time
void display_date_time(char *date_time) 
{
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

// Function to get the current time components
void get_time_components(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) 
{
    time_t now;
    struct tm timeinfo;

    // Get current time
    time(&now);
    localtime_r(&now, &timeinfo);

    // Extract time components
    *hours = timeinfo.tm_hour;
    *minutes = timeinfo.tm_min;
    *seconds = timeinfo.tm_sec;
}

void get_date_components(uint8_t *day, uint8_t *month, uint16_t *year) {
    time_t now;
    struct tm timeinfo;

    // Get current time
    time(&now);
    localtime_r(&now, &timeinfo);

    // Extract date components
    *day = timeinfo.tm_mday;
    *month = timeinfo.tm_mon + 1;
    *year = timeinfo.tm_year + 1900;
}
