#include "ble/ble_utils.h"

#ifndef BLE_SERVICE_MAG_H
#define BLE_SERVICE_MAG_H

extern const uint16_t GATTS_SERVICE_UUID_MAG;
extern const uint16_t GATTS_CHAR_UUID_MAG_X;
extern const uint16_t GATTS_CHAR_UUID_MAG_Y;
extern const uint16_t GATTS_CHAR_UUID_MAG_Z;

extern uint8_t char_value_mag_X[32];
extern uint8_t char_value_mag_Y[32];
extern uint8_t char_value_mag_Z[32];

extern uint16_t mag_handle_table[];

extern const esp_gatts_attr_db_t mag_gatts_db[];

enum mag_enum_gatts {
    IDX_SVC_MAG,
    IDX_CHAR_MAG_X,
    IDX_CHAR_VAL_MAG_X,

    IDX_CHAR_MAG_Y,
    IDX_CHAR_VAL_MAG_Y,

    IDX_CHAR_MAG_Z,
    IDX_CHAR_VAL_MAG_Z,

    NB_MAG_IDX,
};

void mag_x_update_value(float *value);
void mag_y_update_value(float *value);
void mag_z_update_value(float *value);

#endif
