#include "ble/ble_utils.h"

#ifndef BLE_SERVICE_GYRO_H
#define BLE_SERVICE_GYRO_H

extern const uint16_t GATTS_SERVICE_UUID_GYRO;
extern const uint16_t GATTS_CHAR_UUID_GYRO_X;
extern const uint16_t GATTS_CHAR_UUID_GYRO_Y;
extern const uint16_t GATTS_CHAR_UUID_GYRO_Z;

extern uint8_t char_value_gyro_X[32];
extern uint8_t char_value_gyro_Y[32];
extern uint8_t char_value_gyro_Z[32];

extern uint16_t gyro_handle_table[];

extern const esp_gatts_attr_db_t gyro_gatts_db[];

enum gyro_enum_gatts{
    IDX_SVC_GYRO,
    IDX_CHAR_GYRO_X,
    IDX_CHAR_VAL_GYRO_X,

    IDX_CHAR_GYRO_Y,
    IDX_CHAR_VAL_GYRO_Y,

    IDX_CHAR_GYRO_Z,
    IDX_CHAR_VAL_GYRO_Z,

    NB_GYRO_IDX,
};

void gyro_x_update_value(float *value);
void gyro_y_update_value(float *value);
void gyro_z_update_value(float *value);

#endif
