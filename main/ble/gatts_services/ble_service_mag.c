#include "ble/gatts_services/ble_service_mag.h"
#include "ble/ble_utils.h"

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500

const uint16_t GATTS_SERVICE_UUID_MAG = 0x0300;
const uint16_t GATTS_CHAR_UUID_MAG_X = 0x0301;
const uint16_t GATTS_CHAR_UUID_MAG_Y = 0x0302;
const uint16_t GATTS_CHAR_UUID_MAG_Z = 0x0303;

uint8_t char_value_mag_X[32] = {0x00};
uint8_t char_value_mag_Y[32] = {0x00};
uint8_t char_value_mag_Z[32] = {0x00};

uint16_t mag_handle_table[NB_MAG_IDX];

const esp_gatts_attr_db_t mag_gatts_db[NB_MAG_IDX] = {
    // Service Declaration
    [IDX_SVC_MAG]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_MAG), (uint8_t *)&GATTS_SERVICE_UUID_MAG}},

    // Characteristic Declaration
    [IDX_CHAR_MAG_X]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},
    
    // Characteristic Value
    [IDX_CHAR_VAL_MAG_X] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_MAG_X, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_mag_X), (uint8_t *)char_value_mag_X}},

    // Characteristic Declaration
    [IDX_CHAR_MAG_Y]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},

    // Characteristic Value
    [IDX_CHAR_VAL_MAG_Y]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_MAG_Y, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_mag_Y), (uint8_t *)char_value_mag_Y}},

    // Characteristic Declaration
    [IDX_CHAR_MAG_Z]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},

    // Characteristic Value
    [IDX_CHAR_VAL_MAG_Z]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_MAG_Z, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value_mag_Z), (uint8_t *)char_value_mag_Z}},
};

void mag_x_update_value(float *value) {
    update_characteristic_value(mag_handle_table[IDX_CHAR_VAL_MAG_X], value, sizeof(float));
}

void mag_y_update_value(float *value) {
    update_characteristic_value(mag_handle_table[IDX_CHAR_VAL_MAG_Y], value, sizeof(float));
}

void mag_z_update_value(float *value) {
    update_characteristic_value(mag_handle_table[IDX_CHAR_VAL_MAG_Z], value, sizeof(float));
}
