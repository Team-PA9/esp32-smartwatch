#include "ble/ble_utils.h"

#ifndef BLE_SERVICE_TEMP_H
#define BLE_SERVICE_TEMP_H

extern const uint16_t GATTS_SERVICE_UUID_TEMP;
extern const uint16_t GATTS_CHAR_UUID_TEMP;

extern uint16_t temp_handle_table[];

extern const esp_gatts_attr_db_t temp_gatts_db[];

enum temp_enum_gatts {
    IDX_SVC_TEMP,

    IDX_CHAR_TEMP,
    IDX_CHAR_VAL_TEMP,
    
    NB_TEMP_IDX,
};

void temp_update_value(float *value);

#endif
