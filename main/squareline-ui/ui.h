// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: smartwatch

#ifndef _SMARTWATCH_UI_H
#define _SMARTWATCH_UI_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"

// SCREEN: ui_WatchScreen
void ui_WatchScreen_screen_init(void);
extern lv_obj_t *ui_WatchScreen;
extern lv_obj_t *ui_Watchface;
extern lv_obj_t *ui_LittleHand;
extern lv_obj_t *ui_BigHand;
extern lv_obj_t *ui_SecondHand;
// SCREEN: ui_Sensor1Screen
void ui_Sensor1Screen_screen_init(void);
extern lv_obj_t *ui_Sensor1Screen;
extern lv_obj_t *ui_TempChart;
extern lv_obj_t *ui____initial_actions0;

LV_IMG_DECLARE( ui_img_1308839649);   // assets/background-watch.png
LV_IMG_DECLARE( ui_img_little_png);   // assets/little.png
LV_IMG_DECLARE( ui_img_big_png);   // assets/big.png
LV_IMG_DECLARE( ui_img_second_hand_png);   // assets/second_hand.png




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
