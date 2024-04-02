#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "time.h"
#include "rtc.h"

// Functions

#ifndef RTC_PROJ_H
#define RTC_PROJ_H

void tz_init();
void display_date_time(char *date_time);
void get_time_components(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void get_date_components(uint8_t *day, uint8_t *month, uint16_t *year);

#endif