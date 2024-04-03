#ifndef BLE_UTILS_H_
#define BLE_UTILS_H_

#include "esp_gatts_api.h"

#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

extern const uint16_t primary_service_uuid;
extern const uint16_t character_client_config_uuid;
extern const uint16_t character_declaration_uuid;
extern const uint16_t character_description; //0x2901

extern const uint8_t char_prop_notify;
extern const uint8_t char_prop_read;
extern const uint8_t char_prop_read_notify;
extern const uint8_t char_prop_read_write;
extern const uint8_t char_prop_read_write_notify;
extern const uint8_t char_prop_write;
extern const uint8_t char_prop_write_notify;

void update_characteristic_value(uint16_t handle, void *value, size_t valueSize);

#endif /* BLE_UTILS_H_ */
