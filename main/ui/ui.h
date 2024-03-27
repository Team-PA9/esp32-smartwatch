#include <stdio.h>
#include "bsp/esp_wrover_kit.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "squareline-ui/ui.h"

#ifndef UI_H
#define UI_H

// Functions
void screen_init();
void clock_handler();
void cycle_screen();

void lv_tick_task(void *arg);

#endif