// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: smartwatch

#include "ui.h"

void ui_AccelerometerScreen_screen_init(void)
{
ui_AccelerometerScreen = lv_obj_create(NULL);
lv_obj_clear_flag( ui_AccelerometerScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_AccelerometerScreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_AccelerometerScreen, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_AccelerometerChart = lv_chart_create(ui_AccelerometerScreen);
lv_obj_set_width( ui_AccelerometerChart, 249);
lv_obj_set_height( ui_AccelerometerChart, 163);
lv_obj_set_x( ui_AccelerometerChart, 22 );
lv_obj_set_y( ui_AccelerometerChart, 2 );
lv_obj_set_align( ui_AccelerometerChart, LV_ALIGN_CENTER );
lv_chart_set_type( ui_AccelerometerChart, LV_CHART_TYPE_LINE);
lv_chart_set_range( ui_AccelerometerChart, LV_CHART_AXIS_PRIMARY_Y, -2000, 2000);
lv_chart_set_range( ui_AccelerometerChart, LV_CHART_AXIS_SECONDARY_Y, 0, 0);
lv_chart_set_axis_tick( ui_AccelerometerChart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 10, 1, true, 50);
lv_chart_set_axis_tick( ui_AccelerometerChart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 50);
lv_chart_set_axis_tick( ui_AccelerometerChart, LV_CHART_AXIS_SECONDARY_Y, 0, 0, 0, 0, false, 25);



ui_AccelerometerLabel = lv_label_create(ui_AccelerometerScreen);
lv_obj_set_width( ui_AccelerometerLabel, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_AccelerometerLabel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_AccelerometerLabel, 0 );
lv_obj_set_y( ui_AccelerometerLabel, -100 );
lv_obj_set_align( ui_AccelerometerLabel, LV_ALIGN_CENTER );
lv_label_set_text(ui_AccelerometerLabel,"Linear acceleration (g)");
lv_obj_set_style_text_color(ui_AccelerometerLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_text_opa(ui_AccelerometerLabel, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

}
