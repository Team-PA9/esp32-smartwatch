#include "ble/ble_utils.h"

#ifndef BLE_SERVICE_ACCEL_H
#define BLE_SERVICE_ACCEL_H

extern const uint16_t GATTS_SERVICE_UUID_ACCEL;
extern const uint16_t GATTS_CHAR_UUID_ACCEL_X;
extern const uint16_t GATTS_CHAR_UUID_ACCEL_Y;
extern const uint16_t GATTS_CHAR_UUID_ACCEL_Z;

extern uint8_t char_value_accel_X[32];
extern uint8_t char_value_accel_Y[32];
extern uint8_t char_value_accel_Z[32];

extern uint16_t accel_handle_table[];

extern const esp_gatts_attr_db_t accel_gatts_db[];

enum accel_enum_gatts {
    IDX_SVC_ACCEL,
    IDX_CHAR_ACCEL_X,
    IDX_CHAR_VAL_ACCEL_X,

    IDX_CHAR_ACCEL_Y,
    IDX_CHAR_VAL_ACCEL_Y,

    IDX_CHAR_ACCEL_Z,
    IDX_CHAR_VAL_ACCEL_Z,

    NB_ACCEL_IDX,
};

void accel_x_update_value(float *value);
void accel_y_update_value(float *value);
void accel_z_update_value(float *value);

#endif
