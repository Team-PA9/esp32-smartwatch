#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#ifndef BLE_H
#define BLE_H

/* Attributes State Machine */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};

// Functions
void ble_init();

#endif