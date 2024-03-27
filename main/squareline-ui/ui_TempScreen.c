// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: smartwatch

#include "ui.h"

void ui_TempScreen_screen_init(void)
{
ui_TempScreen = lv_obj_create(NULL);
lv_obj_clear_flag( ui_TempScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_TempScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_TempScreen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_TempChart = lv_chart_create(ui_TempScreen);
lv_obj_set_width( ui_TempChart, 265);
lv_obj_set_height( ui_TempChart, 191);
lv_obj_set_x( ui_TempChart, 14 );
lv_obj_set_y( ui_TempChart, -12 );
lv_obj_set_align( ui_TempChart, LV_ALIGN_CENTER );
lv_chart_set_type( ui_TempChart, LV_CHART_TYPE_LINE);
lv_chart_set_range( ui_TempChart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
lv_chart_set_range( ui_TempChart, LV_CHART_AXIS_SECONDARY_Y, 0, 0);
lv_chart_set_axis_tick( ui_TempChart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 10, 1, true, 50);
lv_chart_set_axis_tick( ui_TempChart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 50);
lv_chart_set_axis_tick( ui_TempChart, LV_CHART_AXIS_SECONDARY_Y, 0, 0, 0, 0, false, 25);




}