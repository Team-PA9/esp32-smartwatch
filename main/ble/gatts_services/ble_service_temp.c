#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ble/gatts_services/ble_service_temp.h"
#include "ble/ble_utils.h"

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500

const uint16_t GATTS_SERVICE_UUID_TEMP = 0x0400;
const uint16_t GATTS_CHAR_UUID_TEMP = 0x0401;

uint8_t char_value_temp[32] = {0x00};

uint16_t temp_handle_table[NB_TEMP_IDX];

const esp_gatts_attr_db_t temp_gatts_db[NB_TEMP_IDX] = {
    // Service Declaration
    [IDX_SVC_TEMP]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEMP), (uint8_t *)&GATTS_SERVICE_UUID_TEMP}},

    // Characteristic Declaration
    [IDX_CHAR_TEMP]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},

    // Characteristic Value
    [IDX_CHAR_VAL_TEMP] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEMP, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_temp), (uint8_t *)char_value_temp}},
};

void temp_update_value(float *value) {
    update_characteristic_value(temp_handle_table[IDX_CHAR_VAL_TEMP], value, sizeof(float));
}