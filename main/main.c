#include "esp_log.h"
#include "bsp/esp_wrover_kit.h"
#include "lvgl.h"
#include "ui/ui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "SMARTWATCH"
#define APP_DISP_DEFAULT_BRIGHTNESS 50


static lv_disp_t *display;

/*******************************************************************************
* Private functions
*******************************************************************************/

void lvgl_task(void *pvParameter) {
    // Initialize UI
    bsp_display_lock(0);
    ui_init();
    bsp_display_unlock();

    while(1) {
        // Handle LVGL tasks
        lv_task_handler();
        // Delay for a short period
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    // Initialize display and LVGL
    display = bsp_display_start();

    // Rotate display 90 degrees (landscape mode)
    bsp_display_rotate(display, 90);
    lv_disp_set_rotation(display, LV_DISP_ROT_90);

    // Turn on display backlight
    bsp_display_brightness_set(APP_DISP_DEFAULT_BRIGHTNESS);

    // Create FreeRTOS task for LVGL
    xTaskCreate(&lvgl_task, "lvgl_task", 4096, NULL, 1, NULL);

    ESP_LOGI(TAG, "Example initialization done.");
}
