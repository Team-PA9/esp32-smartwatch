#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"

#include "ble.h"
#include "ble/ble_gatts.h"
#include "ble/ble_gap.h"
#include "ble/ble_utils.h"

#include "ble/gatts_services/ble_service_gyro.h"
#include "ble/gatts_services/ble_service_accel.h"
#include "ble/gatts_services/ble_service_mag.h"
#include "ble/gatts_services/ble_service_temp.h"

#define LOG_TAG "BLE_GATTS"

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define SAMPLE_DEVICE_NAME          "PA9-Team-Smartwatch"
#define SVC_INST_ID                 0

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

extern uint8_t adv_config_done;

extern esp_ble_adv_data_t adv_data;
extern esp_ble_adv_data_t scan_rsp_data;
extern esp_ble_adv_params_t adv_params;

static prepare_type_env_t prepare_write_env;

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst app_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

extern uint16_t gyro_handle_table[NB_GYRO_IDX];
extern uint16_t accel_handle_table[NB_ACCEL_IDX];
extern uint16_t mag_handle_table[NB_MAG_IDX];
extern uint16_t temp_handle_table[NB_TEMP_IDX];

extern const uint16_t GATTS_SERVICE_UUID_GYRO;
extern const uint16_t GATTS_CHAR_UUID_GYRO_X;
extern const uint16_t GATTS_CHAR_UUID_GYRO_Y;
extern const uint16_t GATTS_CHAR_UUID_GYRO_Z;

extern const uint16_t GATTS_SERVICE_UUID_ACCEL;
extern const uint16_t GATTS_CHAR_UUID_ACCEL_X;
extern const uint16_t GATTS_CHAR_UUID_ACCEL_Y;
extern const uint16_t GATTS_CHAR_UUID_ACCEL_Z;

extern const uint16_t GATTS_SERVICE_UUID_MAG;
extern const uint16_t GATTS_CHAR_UUID_MAG_X;
extern const uint16_t GATTS_CHAR_UUID_MAG_Y;
extern const uint16_t GATTS_CHAR_UUID_MAG_Z;

extern const uint16_t GATTS_SERVICE_UUID_TEMP;
extern const uint16_t GATTS_CHAR_UUID_TEMP;

extern const esp_gatts_attr_db_t gyro_gatts_db[NB_GYRO_IDX];
extern const esp_gatts_attr_db_t accel_gatts_db[NB_ACCEL_IDX];
extern const esp_gatts_attr_db_t mag_gatts_db[NB_MAG_IDX];
extern const esp_gatts_attr_db_t temp_gatts_db[NB_TEMP_IDX];

void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(LOG_TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.offset > PREPARE_BUF_MAX_SIZE) {
        status = ESP_GATT_INVALID_OFFSET;
    } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
        status = ESP_GATT_INVALID_ATTR_LEN;
    }
    if (status == ESP_GATT_OK && prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(LOG_TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    }

    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK) {
               ESP_LOGE(LOG_TAG, "Send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(LOG_TAG, "%s, malloc failed", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;

}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf){
        esp_log_buffer_hex(LOG_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(LOG_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:{
            esp_err_t set_dev_name_ret = esp_bt_dev_set_device_name(SAMPLE_DEVICE_NAME);
            if (set_dev_name_ret){
                ESP_LOGE(LOG_TAG, "set device name failed, error code = %x", set_dev_name_ret);
            }
            //config adv data
            esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
            if (ret){
                ESP_LOGE(LOG_TAG, "config adv data failed, error code = %x", ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            //config scan response data
            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret){
                ESP_LOGE(LOG_TAG, "config scan response data failed, error code = %x", ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
            esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gyro_gatts_db, gatts_if, NB_GYRO_IDX, SVC_INST_ID);
            esp_err_t create_attr_ret2 = esp_ble_gatts_create_attr_tab(accel_gatts_db, gatts_if, NB_ACCEL_IDX, SVC_INST_ID);
            esp_err_t create_attr_ret3 = esp_ble_gatts_create_attr_tab(mag_gatts_db, gatts_if, NB_MAG_IDX, SVC_INST_ID);
            esp_err_t create_attr_ret4 = esp_ble_gatts_create_attr_tab(temp_gatts_db, gatts_if, NB_TEMP_IDX, SVC_INST_ID);
            if (create_attr_ret){
                ESP_LOGE(LOG_TAG, "create attr table failed, error code = %x", create_attr_ret);
            }
            if (create_attr_ret2){
                ESP_LOGE(LOG_TAG, "create attr table failed, error code = %x", create_attr_ret2);
            }
            if (create_attr_ret3){
                ESP_LOGE(LOG_TAG, "create attr table failed, error code = %x", create_attr_ret3);
            }
            if (create_attr_ret4){
                ESP_LOGE(LOG_TAG, "create attr table failed, error code = %x", create_attr_ret4);
            }
        }
       	    break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(LOG_TAG, "ESP_GATTS_READ_EVT, handle = %d", param->read.handle);

       	    break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep){
                // the data length of gattc write  must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
                ESP_LOGI(LOG_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
                esp_log_buffer_hex(LOG_TAG, param->write.value, param->write.len);
                /* if (app_handle_table[IDX_CHAR_CFG_A] == param->write.handle && param->write.len == 2){
                    uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                    if (descr_value == 0x0001){
                        ESP_LOGI(LOG_TAG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i % 0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, app_handle_table[IDX_CHAR_VAL_A],
                                                sizeof(notify_data), notify_data, false);
                    }else if (descr_value == 0x0002){
                        ESP_LOGI(LOG_TAG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i % 0xff;
                        }

                        // if want to change the value in server database, call:
                        // esp_ble_gatts_set_attr_value(app_handle_table[IDX_CHAR_VAL_A], sizeof(indicate_data), indicate_data);


                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, app_handle_table[IDX_CHAR_VAL_A],
                                            sizeof(indicate_data), indicate_data, true);
                    }
                    else if (descr_value == 0x0000){
                        ESP_LOGI(LOG_TAG, "notify/indicate disable ");
                    }else{
                        ESP_LOGE(LOG_TAG, "unknown descr value");
                        esp_log_buffer_hex(LOG_TAG, param->write.value, param->write.len);
                    }

                } */
                /* send response when param->write.need_rsp is true*/
                if (param->write.need_rsp){
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                }
            }else{
                /* handle prepare write */
                example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }
      	    break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            // the length of gattc prepare write data must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
            ESP_LOGI(LOG_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            example_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(LOG_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(LOG_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(LOG_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(LOG_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_log_buffer_hex(LOG_TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(LOG_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            esp_ble_gap_start_advertising(&adv_params);
            break;
        /* case ESP_GATTS_CREAT_ATTR_TAB_EVT:{ 
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(LOG_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else {
                ESP_LOGI(LOG_TAG, "create attribute table successfully, the number handle = %d",param->add_attr_tab.num_handle);
                memcpy(gyro_handle_table, param->add_attr_tab.handles, sizeof(gyro_handle_table));
                esp_ble_gatts_start_service(gyro_handle_table[IDX_SVC_GYRO]);

                memcpy(accel_handle_table, param->add_attr_tab.handles, sizeof(accel_handle_table));
                esp_ble_gatts_start_service(accel_handle_table[IDX_SVC_ACCEL]);

                memcpy(mag_handle_table, param->add_attr_tab.handles, sizeof(mag_handle_table));
                esp_ble_gatts_start_service(mag_handle_table[IDX_SVC_MAG]);

                memcpy(temp_handle_table, param->add_attr_tab.handles, sizeof(temp_handle_table));
                esp_ble_gatts_start_service(temp_handle_table[IDX_SVC_TEMP]);
            }
            break;
        } */
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(LOG_TAG,"create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else if(param->add_attr_tab.svc_uuid.uuid.uuid16 == GATTS_SERVICE_UUID_GYRO) {
				if(param->add_attr_tab.num_handle != NB_GYRO_IDX) {
					ESP_LOGE(LOG_TAG,"create attribute table abnormally, num_handle (%d) isn't equal to INFO_NB(%d)", param->add_attr_tab.num_handle, NB_GYRO_IDX);
				}
				else {
					ESP_LOGI(LOG_TAG,"create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
					memcpy(gyro_handle_table, param->add_attr_tab.handles, sizeof(gyro_handle_table));
                    esp_ble_gatts_start_service(gyro_handle_table[IDX_SVC_GYRO]);
				}
            }
            else if(param->add_attr_tab.svc_uuid.uuid.uuid16 == GATTS_SERVICE_UUID_ACCEL) {
                if(param->add_attr_tab.num_handle != NB_ACCEL_IDX) {
                    ESP_LOGE(LOG_TAG,"create attribute table abnormally, num_handle (%d) isn't equal to INFO_NB(%d)", param->add_attr_tab.num_handle, NB_ACCEL_IDX);
                }
                else {
                    ESP_LOGI(LOG_TAG,"create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                    memcpy(accel_handle_table, param->add_attr_tab.handles, sizeof(accel_handle_table));
                    esp_ble_gatts_start_service(accel_handle_table[IDX_SVC_ACCEL]);
                }
            }
            else if(param->add_attr_tab.svc_uuid.uuid.uuid16 == GATTS_SERVICE_UUID_MAG) {
                if(param->add_attr_tab.num_handle != NB_MAG_IDX) {
                    ESP_LOGE(LOG_TAG,"create attribute table abnormally, num_handle (%d) isn't equal to INFO_NB(%d)", param->add_attr_tab.num_handle, NB_MAG_IDX);
                }
                else {
                    ESP_LOGI(LOG_TAG,"create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                    memcpy(mag_handle_table, param->add_attr_tab.handles, sizeof(mag_handle_table));
                    esp_ble_gatts_start_service(mag_handle_table[IDX_SVC_MAG]);
                }
            }
            else if(param->add_attr_tab.svc_uuid.uuid.uuid16 == GATTS_SERVICE_UUID_TEMP) {
                if(param->add_attr_tab.num_handle != NB_TEMP_IDX) {
                    ESP_LOGE(LOG_TAG,"create attribute table abnormally, num_handle (%d) isn't equal to INFO_NB(%d)", param->add_attr_tab.num_handle, NB_TEMP_IDX);
                }
                else {
                    ESP_LOGI(LOG_TAG,"create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                    memcpy(temp_handle_table, param->add_attr_tab.handles, sizeof(temp_handle_table));
                    esp_ble_gatts_start_service(temp_handle_table[IDX_SVC_TEMP]);
                }
            }
            /* END start external services */
            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
    }
}


void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            app_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGE(LOG_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == app_profile_tab[idx].gatts_if) {
                if (app_profile_tab[idx].gatts_cb) {
                    app_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}