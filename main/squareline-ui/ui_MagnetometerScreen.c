// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: smartwatch

#include "ui.h"

void ui_MagnetometerScreen_screen_init(void)
{
ui_MagnetometerScreen = lv_obj_create(NULL);
lv_obj_clear_flag( ui_MagnetometerScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_MagnetometerScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_MagnetometerScreen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_MagnetometerChart = lv_chart_create(ui_MagnetometerScreen);
lv_obj_set_width( ui_MagnetometerChart, 250);
lv_obj_set_height( ui_MagnetometerChart, 191);
lv_obj_set_x( ui_MagnetometerChart, 23 );
lv_obj_set_y( ui_MagnetometerChart, -12 );
lv_obj_set_align( ui_MagnetometerChart, LV_ALIGN_CENTER );
lv_chart_set_type( ui_MagnetometerChart, LV_CHART_TYPE_LINE);
lv_chart_set_range( ui_MagnetometerChart, LV_CHART_AXIS_PRIMARY_Y, -1000, 1000);
lv_chart_set_range( ui_MagnetometerChart, LV_CHART_AXIS_SECONDARY_Y, 0, 0);
lv_chart_set_axis_tick( ui_MagnetometerChart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 10, 1, true, 50);
lv_chart_set_axis_tick( ui_MagnetometerChart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 50);
lv_chart_set_axis_tick( ui_MagnetometerChart, LV_CHART_AXIS_SECONDARY_Y, 0, 0, 0, 0, false, 25);




}
