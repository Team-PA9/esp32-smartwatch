/** ----------------------------------------------------------------------------
 * Project:     ESP32 - SmartWatch
 * File:        main.c
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Author:      PA9 TEAM
 * Modified:    11/03/2024
 * Framework:   ESP32 (Wrover Kit) v5.1.2
 * -------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
 * PART 0 : Includes
 * -------------------------------------------------------------------------- */
#include <stdio.h>
// --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
#include "bsp/esp_wrover_kit.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "sdkconfig.h"
// --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
#include "sensors/sensors.h"
#include "sensors/lis2mdl_reg.h"
#include "sensors/lsm6dso_reg.h"
#include "ui/ui.h"


/* -----------------------------------------------------------------------------
 * PART 1 : Global Defines, Variables & Structures
 * -------------------------------------------------------------------------- */
static const char *TAG = "SMARTWATCH";

// Display
#define APP_DISP_DEFAULT_BRIGHTNESS 50
static lv_disp_t *display;

// Sensors
extern stmdev_ctx_t lsm6dso_dev_ctx, lis2mdl_dev_ctx;
extern uint8_t LSM6DSO_whoamI, LIS2MDL_whoamI;
extern bool LSM6DSO_OK, LIS2MDL_OK;


/* -----------------------------------------------------------------------------
 * PART 2 : Global Handlers
 * -------------------------------------------------------------------------- */
TaskHandle_t xHandle_lvgl = NULL;
TaskHandle_t xHandle_whoami = NULL;
TaskHandle_t xHandle_getdata = NULL;


/* -----------------------------------------------------------------------------
 * PART 3 : Private Functions
 * -------------------------------------------------------------------------- */

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Task : LVGL initialize and handle
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
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

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Task : Get data from sensors
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void get_data_task(void *args) {
    while (1) {
        if (LSM6DSO_OK == 1) { get_LSM6DSO(); }
        if (LIS2MDL_OK == 1) { get_LIS2MDL(); }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Task : WhoAmI, check if sensor is detected on I2C
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void whoami_task(void *args) {
    static const char *TAG = "WhoAmI";

    while (1) {
        ESP_LOGI(TAG, "Master check sensors ---");

        // LSM6DSO ID
        lsm6dso_device_id_get(&lsm6dso_dev_ctx, &LSM6DSO_whoamI);
        if (LSM6DSO_whoamI != LSM6DSO_ID) { ESP_LOGE(TAG, "LSM6DSO not find"); }
        else {
            printf("Product ID= %d \n", LSM6DSO_whoamI);
            LSM6DSO_OK = 1;
            }

        // LIS2MDL ID
        lis2mdl_device_id_get(&lis2mdl_dev_ctx, &LIS2MDL_whoamI);
        if (LIS2MDL_whoamI != LIS2MDL_ID) { ESP_LOGE(TAG, "LIS2MDL not find"); }
        else {
            printf("Product ID= %d \n", LIS2MDL_whoamI);
            LIS2MDL_OK = 1;
            }

        xTaskCreate(get_data_task, "get_data_task", 2048, NULL, 1, &xHandle_getdata);

        ESP_LOGI(TAG, "End of check ---");
        vTaskDelete(xHandle_whoami);
        fflush(stdout);
  }
}


/* -----------------------------------------------------------------------------
 * PART 4 : app_main()
 * -------------------------------------------------------------------------- */
void app_main(void) {
    ESP_LOGI(TAG, "Started app_main() ---");

    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Step 0: Initialize Display & LVGL
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    printf("Initialize display and LVGL ...\n");
    display = bsp_display_start();
    // Rotate display 90 degrees (landscape mode)
    bsp_display_rotate(display, 90);
    lv_disp_set_rotation(display, LV_DISP_ROT_90);
    // Turn on display backlight
    bsp_display_brightness_set(APP_DISP_DEFAULT_BRIGHTNESS);
    // Create FreeRTOS task for LVGL
    xTaskCreate(&lvgl_task, "lvgl_task", 4096, NULL, 1, &xHandle_lvgl);

    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Step 1: Initialize I2C & Sensors
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    printf("Initialize I2C master... \n");
    ESP_ERROR_CHECK(i2c_master_init());
    printf("Initialize LSM6DSO sensor... \n");
    ESP_ERROR_CHECK(lsm6dso_init());
    printf("Initialize LIS2MDL sensor... \n");
	ESP_ERROR_CHECK(lis2mdl_init());
    // Create whoamI task for sensors on I2C
    xTaskCreate(whoami_task, "whoami_task", 2048, NULL, 1, &xHandle_whoami);

    ESP_LOGI(TAG, "Ended app_main() ---");
    fflush(stdout);
}
