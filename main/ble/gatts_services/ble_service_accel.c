#include "ble/gatts_services/ble_service_accel.h"
#include "ble/ble_utils.h"

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500

const uint16_t GATTS_SERVICE_UUID_ACCEL = 0x0200;
const uint16_t GATTS_CHAR_UUID_ACCEL_X = 0x0201;
const uint16_t GATTS_CHAR_UUID_ACCEL_Y = 0x0202;
const uint16_t GATTS_CHAR_UUID_ACCEL_Z = 0x0203;

uint8_t char_value_accel_X[32] = {0x00};
uint8_t char_value_accel_Y[32] = {0x00};
uint8_t char_value_accel_Z[32] = {0x00};

uint16_t accel_handle_table[NB_ACCEL_IDX];

const esp_gatts_attr_db_t accel_gatts_db[NB_ACCEL_IDX] = {
    // Service Declaration
    [IDX_SVC_ACCEL]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_ACCEL), (uint8_t *)&GATTS_SERVICE_UUID_ACCEL}},

    // Characteristic Declaration
    [IDX_CHAR_ACCEL_X]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},
    
    // Characteristic Value
    [IDX_CHAR_VAL_ACCEL_X] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_ACCEL_X, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_accel_X), (uint8_t *)char_value_accel_X}},

    // Characteristic Declaration
    [IDX_CHAR_ACCEL_Y]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Characteristic Value
    [IDX_CHAR_VAL_ACCEL_Y]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_ACCEL_Y, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_accel_Y), (uint8_t *)char_value_accel_Y}},

    // Characteristic Declaration
    [IDX_CHAR_ACCEL_Z]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Characteristic Value
    [IDX_CHAR_VAL_ACCEL_Z]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_ACCEL_Z, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_accel_Z), (uint8_t *)char_value_accel_Z}},
};

void accel_x_update_value(float *value) {
    update_characteristic_value(accel_handle_table[IDX_CHAR_VAL_ACCEL_X], value, sizeof(float));
}

void accel_y_update_value(float *value) {
    update_characteristic_value(accel_handle_table[IDX_CHAR_VAL_ACCEL_Y], value, sizeof(float));
}

void accel_z_update_value(float *value) {
    update_characteristic_value(accel_handle_table[IDX_CHAR_VAL_ACCEL_Z], value, sizeof(float));
}