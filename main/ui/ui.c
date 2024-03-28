#include <stdio.h>
#include "bsp/esp_wrover_kit.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "ui/ui.h"
#include "squareline-ui/ui.h"

// Includes
#define LV_TICK_PERIOD_MS 1
#define APP_DISP_DEFAULT_BRIGHTNESS 100

// Variables
static lv_disp_t *display;
static uint8_t screen = 0;

extern lv_obj_t *ui_SecondHand; // Seconds
extern lv_obj_t *ui_BigHand; // Minutes
extern lv_obj_t *ui_LittleHand; // Hours

extern lv_obj_t *ui_WatchScreen;
extern lv_obj_t *ui_CalendarScreen;
extern lv_obj_t *ui_TempScreen;
extern lv_obj_t *ui_AccelerometerScreen;
extern lv_obj_t *ui_GyroScreen;
extern lv_obj_t *ui_MagnetometerScreen;

extern lv_obj_t *ui_TempChart;
lv_chart_series_t* ui_TempChart_series_1;
static lv_coord_t ui_TempChart_series_1_array[] = { 0,0,0,0,0,0,0,0,0,0 };

extern lv_obj_t *ui_AccelerometerChart;
lv_chart_series_t* ui_AccelerometerChart_series_1;
static lv_coord_t ui_AccelerometerChart_series_1_array[] = { 0,0,0,0,0,0,0,0,0,0 };
lv_chart_series_t* ui_AccelerometerChart_series_2;
static lv_coord_t ui_AccelerometerChart_series_2_array[] = { 0,0,0,0,0,0,0,0,0,0 };
lv_chart_series_t* ui_AccelerometerChart_series_3;
static lv_coord_t ui_AccelerometerChart_series_3_array[] = { 0,0,0,0,0,0,0,0,0,0 };

extern lv_obj_t *ui_GyroChart;
lv_chart_series_t* ui_GyroChart_series_1;
static lv_coord_t ui_GyroChart_series_1_array[] = { 0,0,0,0,0,0,0,0,0,0 };
lv_chart_series_t* ui_GyroChart_series_2;
static lv_coord_t ui_GyroChart_series_2_array[] = { 0,0,0,0,0,0,0,0,0,0 };
lv_chart_series_t* ui_GyroChart_series_3;
static lv_coord_t ui_GyroChart_series_3_array[] = { 0,0,0,0,0,0,0,0,0,0 };

extern lv_obj_t *ui_MagnetometerChart;
lv_chart_series_t* ui_MagnetometerChart_series_1;
static lv_coord_t ui_MagnetometerChart_series_1_array[] = { 0,0,0,0,0,0,0,0,0,0 };
lv_chart_series_t* ui_MagnetometerChart_series_2;
static lv_coord_t ui_MagnetometerChart_series_2_array[] = { 0,0,0,0,0,0,0,0,0,0 };
lv_chart_series_t* ui_MagnetometerChart_series_3;
static lv_coord_t ui_MagnetometerChart_series_3_array[] = { 0,0,0,0,0,0,0,0,0,0 };

void screen_init() {
    display = bsp_display_start();

    bsp_display_rotate(display, 90); //Rotate display in landscape mode (90°)
    lv_disp_set_rotation(display, LV_DISP_ROT_90);
    bsp_display_brightness_set(APP_DISP_DEFAULT_BRIGHTNESS);

    bsp_display_lock(0);
    ui_init();

    // RED 0xE74C3C
    // GREEN 0x2ECC71
    // BLUE 0x3498DB

    ui_TempChart_series_1 = lv_chart_add_series(ui_TempChart, lv_color_hex(0xE74C3C), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_TempChart, ui_TempChart_series_1, ui_TempChart_series_1_array);
    
    ui_AccelerometerChart_series_1 = lv_chart_add_series(ui_AccelerometerChart, lv_color_hex(0xE74C3C), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_AccelerometerChart, ui_AccelerometerChart_series_1, ui_AccelerometerChart_series_1_array);
    ui_AccelerometerChart_series_2 = lv_chart_add_series(ui_AccelerometerChart, lv_color_hex(0x2ECC71), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_AccelerometerChart, ui_AccelerometerChart_series_2, ui_AccelerometerChart_series_2_array);
    ui_AccelerometerChart_series_3 = lv_chart_add_series(ui_AccelerometerChart, lv_color_hex(0x3498DB), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_AccelerometerChart, ui_AccelerometerChart_series_3, ui_AccelerometerChart_series_3_array);

    
    ui_GyroChart_series_1 = lv_chart_add_series(ui_GyroChart, lv_color_hex(0xE74C3C), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_GyroChart, ui_GyroChart_series_1, ui_GyroChart_series_1_array);
    ui_GyroChart_series_2 = lv_chart_add_series(ui_GyroChart, lv_color_hex(0x2ECC71), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_GyroChart, ui_GyroChart_series_2, ui_GyroChart_series_2_array);
    ui_GyroChart_series_3 = lv_chart_add_series(ui_GyroChart, lv_color_hex(0x3498DB), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_GyroChart, ui_GyroChart_series_3, ui_GyroChart_series_3_array);


    ui_MagnetometerChart_series_1 = lv_chart_add_series(ui_MagnetometerChart, lv_color_hex(0xE74C3C), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_MagnetometerChart, ui_MagnetometerChart_series_1, ui_MagnetometerChart_series_1_array);
    ui_MagnetometerChart_series_2 = lv_chart_add_series(ui_MagnetometerChart, lv_color_hex(0x2ECC71), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_MagnetometerChart, ui_MagnetometerChart_series_2, ui_MagnetometerChart_series_2_array);
    ui_MagnetometerChart_series_3 = lv_chart_add_series(ui_MagnetometerChart, lv_color_hex(0x3498DB), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui_MagnetometerChart, ui_MagnetometerChart_series_3, ui_MagnetometerChart_series_3_array);

    bsp_display_unlock();

    const esp_timer_create_args_t periodic_timer_args = {
		.callback = &lv_tick_task,
		.name = "periodic_gui"
	};
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
}

void clock_handler() {
    if (screen == 0) {
        static uint8_t sec = 0;
        static uint8_t min = 0;
        static uint8_t hour = 0;
        sec++;

        if (sec == 60) {
            sec = 0;
            min++;
        }

        if (min == 60) {
            min = 0;
            hour++;
        }

        if (hour == 24) {
            hour = 0;
        }

        // Rotate the second hand by 6 degrees each second
        lv_img_set_angle(ui_SecondHand, (sec * 6) * 10);

        // Rotate the minute hand by 6 degrees each minute
        lv_img_set_angle(ui_BigHand, (min * 6) * 10);

        // Rotate the hour hand by 30 degrees each hour
        lv_img_set_angle(ui_LittleHand, (hour * 30) * 10);
    }
}

void cycle_screen() {
    screen++;
    bsp_display_lock(0);
    switch (screen) {
        case 0:
            lv_scr_load_anim(ui_WatchScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
            break;
        case 1:
            lv_scr_load_anim(ui_TempScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
            break;
        case 2:
            lv_scr_load_anim(ui_GyroScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
            break;
        case 3:
            lv_scr_load_anim(ui_AccelerometerScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
            break;
        case 4:
            lv_scr_load_anim(ui_MagnetometerScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false); 
            break;
        case 5:
             lv_scr_load_anim(ui_CalendarScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
            break;
        default:
            screen = 0;
            lv_scr_load_anim(ui_WatchScreen, LV_SCR_LOAD_ANIM_FADE_OUT, 1000, 0, false);
            break;
    }
    bsp_display_unlock();
}

void update_sensor_chart(const char* sensor, int series_index, float* value) {
    if (strcmp(sensor, "temp") == 0 && screen == 1) {
        lv_chart_set_next_value(ui_TempChart, ui_TempChart_series_1, *value);
    } else if (strcmp(sensor, "gyro") == 0 && screen == 2) {
        if (series_index == 1) {
            lv_chart_set_next_value(ui_GyroChart, ui_GyroChart_series_1, *value / 1000.0f);
        } else if (series_index == 2) {
            lv_chart_set_next_value(ui_GyroChart, ui_GyroChart_series_2, *value / 1000.0f);
        } else if (series_index == 3) {
            lv_chart_set_next_value(ui_GyroChart, ui_GyroChart_series_3, *value / 1000.0f);
        } else if (series_index == 3) {
            
        }
    } else if (strcmp(sensor, "accelerometer") == 0 && screen == 3) {
        if (series_index == 1) {
            lv_chart_set_next_value(ui_AccelerometerChart, ui_AccelerometerChart_series_1, *value);
        } else if (series_index == 2) {
            lv_chart_set_next_value(ui_AccelerometerChart, ui_AccelerometerChart_series_2, *value);
        } else if (series_index == 3) {
            lv_chart_set_next_value(ui_AccelerometerChart, ui_AccelerometerChart_series_3, *value);
        } 
    } else if (strcmp(sensor, "magnetometer") == 0 && screen == 4) {
        if (series_index == 1) {
            lv_chart_set_next_value(ui_MagnetometerChart, ui_MagnetometerChart_series_1, *value / 1000.0f);
        } else if (series_index == 2) {
            lv_chart_set_next_value(ui_MagnetometerChart, ui_MagnetometerChart_series_2, *value / 1000.0f);
        } else if (series_index == 3) {
            lv_chart_set_next_value(ui_MagnetometerChart, ui_MagnetometerChart_series_3, *value / 1000.0f);
        }
    }
}

void lv_tick_task(void *arg) {
    lv_tick_inc(LV_TICK_PERIOD_MS);
}