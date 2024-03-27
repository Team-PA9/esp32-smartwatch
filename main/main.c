/** ----------------------------------------------------------------------------
 * Project:     ESP32 - SmartWatch
 * File:        main.c
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Author:      PA9 TEAM
 * Modified:    14/03/2024
 * Status:      In progress
 * Framework:   ESP32 (Wrover Kit) v5.1.2
 * -------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
 * PART 0 : Includes
 * -------------------------------------------------------------------------- */
#include <stdio.h>
// --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
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

// Sensors
extern stmdev_ctx_t lsm6dso_dev_ctx, lis2mdl_dev_ctx;
extern uint8_t LSM6DSO_whoamI, LIS2MDL_whoamI;

// GPIOs
#define BootBtn 0

/* -----------------------------------------------------------------------------
 * PART 2 : Global Handlers
 * -------------------------------------------------------------------------- */
QueueSetHandle_t QueueSet_Sem = NULL;
SemaphoreHandle_t xSem_acq = NULL;
SemaphoreHandle_t xSem_display = NULL;
SemaphoreHandle_t xSem_clock = NULL;
SemaphoreHandle_t xSem_btn = NULL;
TaskHandle_t xHdl_ME = NULL;

/* -----------------------------------------------------------------------------
 * PART 3 : Private Functions Declarations
 * -------------------------------------------------------------------------- */
void ME(void *pvParameter);
void display_timer_init(uint64_t period);
static bool display_timer_callback(gptimer_handle_t timer, const
                                   gptimer_alarm_event_data_t *edata,
                                   void *user_ctx);
void acq_timer_init(uint64_t period);
static bool acq_timer_callback(gptimer_handle_t timer, const
                               gptimer_alarm_event_data_t *edata,
                               void *user_ctx);
void clock_timer_init(uint64_t period);
static bool clock_timer_callback(gptimer_handle_t timer, const
                                   gptimer_alarm_event_data_t *edata,
                                   void *user_ctx);
static void gpio_IRQ_handler(void *args);

/* -----------------------------------------------------------------------------
 * PART 4 : app_main()
 * -------------------------------------------------------------------------- */
void app_main(void) {
    ESP_LOGI(TAG, "Started app_main() ---");

    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Step 1: Initialization
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--

    gpio_reset_pin(BootBtn);
	gpio_set_direction(BootBtn, GPIO_MODE_INPUT);
	gpio_set_intr_type(BootBtn, GPIO_INTR_NEGEDGE);

    // Step 1.1 : Peripherals
    printf("Initialize I2C master and sensors... \n");
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_ERROR_CHECK(lsm6dso_init());
	ESP_ERROR_CHECK(lis2mdl_init());
    ESP_ERROR_CHECK(whoami_check());

    // Step 1.2 : Semaphore, Queue
    printf("Initialize Semaphore and Queue... \n");
    xSem_display = xSemaphoreCreateBinary();
    xSem_acq = xSemaphoreCreateBinary();
    xSem_clock = xSemaphoreCreateBinary();
    xSem_btn = xSemaphoreCreateBinary();
    QueueSet_Sem = xQueueCreateSet(4);
    xQueueAddToSet(xSem_display, QueueSet_Sem);
    xQueueAddToSet(xSem_acq, QueueSet_Sem);
    xQueueAddToSet(xSem_clock, QueueSet_Sem);
    xQueueAddToSet(xSem_btn, QueueSet_Sem);

    // Step 1.3 : Display, LVGL & UI
    printf("Initialize display, LVGL and UI...\n");
    screen_init();

    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Step 2: Tasks installation
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    xTaskCreate(ME, "ME", 4096, NULL, 1, &xHdl_ME);

    gpio_install_isr_service(0);
	gpio_isr_handler_add(BootBtn, gpio_IRQ_handler,(void *)BootBtn);

    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Step 3: Interrupts Routines and Timers IRQ
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    acq_timer_init(1000 * 1000);    // Acquisition period : 1s
    display_timer_init(1000 * 300); // Display period : 300ms
    clock_timer_init(1000 * 1000); // Test second hand timer
    
    ESP_LOGI(TAG, "Ended app_main() ---");
    fflush(stdout);
}

/* -----------------------------------------------------------------------------
 * PART 5 : ME Task
 * -------------------------------------------------------------------------- */
void ME(void *pvParameter) {
    char *TAG = "ME";
    QueueSetMemberHandle_t xSemReceived;

    while (1) {
        // Wait for a semaphore to be available
        xSemReceived = xQueueSelectFromSet(QueueSet_Sem, portMAX_DELAY);
        // Check which semaphore was received
        if (xSemReceived == xSem_acq) {
            ESP_LOGW(TAG, "Acquire data from sensors");
            xSemaphoreTake(xSem_acq, 0);
            get_LSM6DSO();
            get_LIS2MDL();
        } 
        else if (xSemReceived == xSem_clock) {
            ESP_LOGW(TAG, "Clock timer");
            xSemaphoreTake(xSem_clock, 0);
            clock_handler();
        }
        else if (xSemReceived == xSem_btn) {
            ESP_LOGW(TAG, "Cycle screen");
            xSemaphoreTake(xSem_btn, 0);
            cycle_screen();
        }
        else if (xSemReceived == xSem_display) {
            ESP_LOGW(TAG, "Update display");
            xSemaphoreTake(xSem_display, 0);
            lv_task_handler();
        }
    }
}

/* -----------------------------------------------------------------------------
 * PART 6 : Private Functions & Tasks Definitions
 * -------------------------------------------------------------------------- */
/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : display_timer_init('period in µs')
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void display_timer_init(uint64_t period) {
    gptimer_handle_t timer_display_hdl = NULL;
    gptimer_config_t timer_display_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000*1000, // 1MHz
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_display_cfg, &timer_display_hdl));
    gptimer_alarm_config_t alarm_display_cfg = {
        .alarm_count = period,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_display_hdl,
                                             &alarm_display_cfg));
    gptimer_event_callbacks_t timer_display_cbs = {
        .on_alarm = display_timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_display_hdl,
                                                     &timer_display_cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(timer_display_hdl));
    ESP_ERROR_CHECK(gptimer_start(timer_display_hdl));
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : display_timer_callback()
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
static bool IRAM_ATTR display_timer_callback(gptimer_handle_t timer, const
                                   gptimer_alarm_event_data_t *edata,
                                   void *user_ctx) {
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSem_display, &xHigherPriorityTaskWoken);
    return true;
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : acq_timer_init('period in µs')
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void acq_timer_init(uint64_t period) {
    gptimer_handle_t timer_acq_hdl = NULL;
    gptimer_config_t timer_acq_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000*1000, // 1MHz
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_acq_cfg, &timer_acq_hdl));
    gptimer_alarm_config_t alarm_acq_cfg = {
        .alarm_count = period,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_acq_hdl, &alarm_acq_cfg));
    gptimer_event_callbacks_t timer_acq_cbs = {
        .on_alarm = acq_timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_acq_hdl,
                                                     &timer_acq_cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(timer_acq_hdl));
    ESP_ERROR_CHECK(gptimer_start(timer_acq_hdl));
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : acq_timer_callback()
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
static bool IRAM_ATTR acq_timer_callback(gptimer_handle_t timer, const
                                         gptimer_alarm_event_data_t *edata,
                                         void *user_ctx) {
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSem_acq, &xHigherPriorityTaskWoken);
    return true;
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : clock_timer_init('period in µs')
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void clock_timer_init(uint64_t period) {
    gptimer_handle_t timer_clock_hdl = NULL;
    gptimer_config_t timer_clock_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000*1000, // 1MHz
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_clock_cfg, &timer_clock_hdl));
    gptimer_alarm_config_t alarm_clock_cfg = {
        .alarm_count = period,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_clock_hdl,
                                             &alarm_clock_cfg));
    gptimer_event_callbacks_t timer_clock_cbs = {
        .on_alarm = clock_timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_clock_hdl,
                                                     &timer_clock_cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(timer_clock_hdl));
    ESP_ERROR_CHECK(gptimer_start(timer_clock_hdl));
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : clock_timer_callback()
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
static bool IRAM_ATTR clock_timer_callback(gptimer_handle_t timer, const
                                   gptimer_alarm_event_data_t *edata,
                                   void *user_ctx) {
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSem_clock, &xHigherPriorityTaskWoken);
    return true;
}

static void IRAM_ATTR gpio_IRQ_handler(void *args) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xSem_btn, &xHigherPriorityTaskWoken);
}