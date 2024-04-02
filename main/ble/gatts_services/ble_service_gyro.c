#include "ble/gatts_services/ble_service_gyro.h"
#include "ble/ble_utils.h"

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500

const uint16_t GATTS_SERVICE_UUID_GYRO = 0x0100;
const uint16_t GATTS_CHAR_UUID_GYRO_X = 0x0101;
const uint16_t GATTS_CHAR_UUID_GYRO_Y = 0x0102;
const uint16_t GATTS_CHAR_UUID_GYRO_Z = 0x0103;

uint8_t char_value_gyro_X[32] = {0x00};
uint8_t char_value_gyro_Y[32] = {0x00};
uint8_t char_value_gyro_Z[32] = {0x00};

uint16_t gyro_handle_table[NB_GYRO_IDX];

const esp_gatts_attr_db_t gyro_gatts_db[NB_GYRO_IDX] = {
    // Service Declaration
    [IDX_SVC_GYRO]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_GYRO), (uint8_t *)&GATTS_SERVICE_UUID_GYRO}},

    // Characteristic Declaration
    [IDX_CHAR_GYRO_X]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},
    
    // Characteristic Value
    [IDX_CHAR_VAL_GYRO_X] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_GYRO_X, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_gyro_X), (uint8_t *)char_value_gyro_X}},

    // Characteristic Declaration
    [IDX_CHAR_GYRO_Y]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Characteristic Value
    [IDX_CHAR_VAL_GYRO_Y]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_GYRO_Y, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_gyro_Y), (uint8_t *)char_value_gyro_Y}},

    // Characteristic Declaration
    [IDX_CHAR_GYRO_Z]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Characteristic Value
    [IDX_CHAR_VAL_GYRO_Z]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_GYRO_Z, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_gyro_Z), (uint8_t *)char_value_gyro_Z}},
};

void gyro_x_update_value(float *value) {
    update_characteristic_value(gyro_handle_table[IDX_CHAR_VAL_GYRO_X], value, sizeof(float));
}

void gyro_y_update_value(float *value) {
    update_characteristic_value(gyro_handle_table[IDX_CHAR_VAL_GYRO_Y], value, sizeof(float));
}

void gyro_z_update_value(float *value) {
    update_characteristic_value(gyro_handle_table[IDX_CHAR_VAL_GYRO_Z], value, sizeof(float));
}