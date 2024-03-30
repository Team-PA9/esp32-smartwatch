#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "ble.h"
#include "ble/ble_gap.h"

#ifndef BLE_GATTS_H
#define BLE_GATTS_H

#define ESP_APP_ID 0x55 // User defined ID it's to identify the profile app, here we have only one profile so one ID is enough

typedef struct {
    uint8_t *prepare_buf;
    int prepare_len;
} prepare_type_env_t;

void gatts_profile_event_handler(esp_gatts_cb_event_t event,esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#endif