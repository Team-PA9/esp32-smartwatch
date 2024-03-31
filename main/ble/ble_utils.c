#include "ble_utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_VALUE_STRING_LENGTH 32 // Maximum length of the string representation of the value
#define ERROR_MESSAGE_TOO_LONG_FLOAT "Too long float"

const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
const uint16_t character_description        = ESP_GATT_UUID_CHAR_DESCRIPTION;

const uint8_t char_prop_notify            = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
const uint8_t char_prop_read              = ESP_GATT_CHAR_PROP_BIT_READ;
const uint8_t char_prop_read_notify       = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
const uint8_t char_prop_read_write        = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
const uint8_t char_prop_write             = ESP_GATT_CHAR_PROP_BIT_WRITE;
const uint8_t char_prop_write_notify      = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

void update_characteristic_value(uint16_t handle, void *value, size_t valueSize) {
    if (valueSize != sizeof(float)) {
        // Handle only float values
        return;
    }
    
    // Convert the numeric value to a string with two decimal places
    char valueString[MAX_VALUE_STRING_LENGTH];
    int charsWritten = snprintf(valueString, MAX_VALUE_STRING_LENGTH, "%.2f", *(float *)value);

    if (charsWritten < 0 || charsWritten >= MAX_VALUE_STRING_LENGTH) {
        // snprintf encountered an error or the formatted string is too long
        // Set error message instead
        esp_ble_gatts_set_attr_value(handle, strlen(ERROR_MESSAGE_TOO_LONG_FLOAT), (uint8_t *)ERROR_MESSAGE_TOO_LONG_FLOAT);
        return;
    }

    // Convert the string to a UTF-8 representation
    uint8_t utf8Value[MAX_VALUE_STRING_LENGTH];
    size_t length = strlen(valueString);
    for (size_t i = 0; i < length; i++) {
        utf8Value[i] = (uint8_t)valueString[i];
    }

    // Set the attribute value with the UTF-8 representation
    esp_ble_gatts_set_attr_value(handle, length, utf8Value);
}


#include "ble_utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
