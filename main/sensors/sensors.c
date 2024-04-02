/** ----------------------------------------------------------------------------
 * Project:     ESP32 - SmartWatch
 * File:        sensors.c
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Author:      PA9 TEAM
 * Modified:    11/03/2024
 * Status:      OK
 * Framework:   ESP32 (Wrover Kit) v5.1.2
 * -------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
 * PART 0 : Includes
 * -------------------------------------------------------------------------- */
#include <string.h>
// --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
#include "driver/i2c.h"
#include "esp_log.h"
// --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
#include "sensors.h"
#include "sensors/lis2mdl_reg.h"
#include "sensors/lsm6dso_reg.h"

#include "ui/ui.h"

#include "squareline-ui/ui.h"
#include "lvgl.h"

#include "ble/gatts_services/ble_service_gyro.h"
#include "ble/gatts_services/ble_service_accel.h"
#include "ble/gatts_services/ble_service_mag.h"
#include "ble/gatts_services/ble_service_temp.h"

/* -----------------------------------------------------------------------------
 * PART 1 : Global Defines, Variables & Structures
 * -------------------------------------------------------------------------- */
// I2C
#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL                                 /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA                                 /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM)                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY                          /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                                             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                                             /*!< I2C master doesn't need buffer */

#define LIS2MDL_ADDR_7BITS 0x1e													/*!< LSM6DSO I2C Address : 8bits = 3C (0011 1100) -> 7bits = 1E (001 1110) */
#define LSM6DSO_ADDR_7BITS 0x6b                                                 /*!< LSM6DSO I2C Address : 8bits = D4 (1110 0110) -> 7bits = 73 (111 0011) */
#define SENSOR_BUS I2C_MASTER_NUM                                               /*!< I2C port number for master dev */
#define WRITE_BIT I2C_MASTER_WRITE                                              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                                                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                                                        /*!< I2C master will check ack from slave */
#define ACK_CHECK_DIS 0x0                                                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                                                             /*!< I2C ack value */
#define NACK_VAL 0x1                                                            /*!< I2C nack value */

// LSM6MDL
stmdev_ctx_t lsm6dso_dev_ctx;
uint8_t LSM6DSO_whoamI, LSM6DSO_rst;
int16_t LSM6DSO_raw_acceleration[3];
int16_t LSM6DSO_raw_angular_rate[3];
int16_t LSM6DSO_raw_temperature;
float LSM6DSO_acceleration_mg[3];
float LSM6DSO_angular_rate_mdps[3];
float LSM6DSO_temperature_degC;

// LIS2MDL
stmdev_ctx_t lis2mdl_dev_ctx;
uint8_t LIS2MDL_whoamI;
int16_t LIS2MDL_raw_magnetic[3];
float LIS2MDL_magnetic_mG[3];

// Log Arrays & Indexs
float log_temperature[100];
float log_magnetic_X[100];
float log_magnetic_Y[100];
float log_magnetic_Z[100];
float log_acceleration_X[100];
float log_acceleration_Y[100];
float log_acceleration_Z[100];
float log_angular_rate_X[100];
float log_angular_rate_Y[100];
float log_angular_rate_Z[100];

uint8_t mag_index = 0;
uint8_t acc_index = 0;
uint8_t gyro_index = 0;
uint8_t temp_index = 0;

// Others
uint8_t tx_buffer[1000];

extern lv_obj_t *ui_TempChart;
extern lv_chart_series_t *ui_TempChart_series_1;

extern lv_obj_t *ui_AccelerometerChart;
extern lv_chart_series_t *ui_AccelerometerChart_series_1;
extern lv_chart_series_t *ui_AccelerometerChart_series_2;
extern lv_chart_series_t *ui_AccelerometerChart_series_3;

extern lv_obj_t *ui_GyroChart;
extern lv_chart_series_t *ui_GyroChart_series_1;
extern lv_chart_series_t *ui_GyroChart_series_2;
extern lv_chart_series_t *ui_GyroChart_series_3;

extern lv_obj_t *ui_MagnetometerChart;
extern lv_chart_series_t *ui_MagnetometerChart_series_1;
extern lv_chart_series_t *ui_MagnetometerChart_series_2;
extern lv_chart_series_t *ui_MagnetometerChart_series_3;

/* -----------------------------------------------------------------------------
 * PART 2 : Private Functions
 * -------------------------------------------------------------------------- */

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Initialize I2C master
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
esp_err_t i2c_master_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) { return err; }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Read data from the sensor with I2C communication.
 * _____________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte (register address) + ack |
 * --------|---------------------------|---------------------------------------|
 *      _________________________________________________________________
 * ...   start | slave_addr + rd_bit + ack | read n bytes + nack | stop |
 *      -------|---------------------------|---------------------|------|
 *
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
int32_t i2c_master_read_LSM6DSO(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size) {
    if (size == 0) { return ESP_OK; }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LSM6DSO_ADDR_7BITS << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LSM6DSO_ADDR_7BITS << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1) { i2c_master_read(cmd, data_rd, size - 1, ACK_VAL); }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

int32_t i2c_master_read_LIS2MDL(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_rd, uint16_t size) {
    if (size == 0) { return ESP_OK; }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LIS2MDL_ADDR_7BITS << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LIS2MDL_ADDR_7BITS << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    if (size > 1) { i2c_master_read(cmd, data_rd, size - 1, ACK_VAL); }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Write data to the sensor with I2C communication.
 * _____________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte (register address) + ack |
 * --------|---------------------------|---------------------------------------|
 *      ______________________________
 * ...   write n bytes + ack  | stop |
 *      ----------------------|------|
 *
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
int32_t i2c_master_write_LSM6DSO(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LSM6DSO_ADDR_7BITS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

int32_t i2c_master_write_LIS2MDL(uint8_t i2c_num, uint8_t regaddr, uint8_t *data_wr, uint16_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LIS2MDL_ADDR_7BITS << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regaddr, ACK_CHECK_EN);

    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Initialize LSM6DSO sensor
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
esp_err_t lsm6dso_init(void) {
	lsm6dso_dev_ctx.write_reg = i2c_master_write_LSM6DSO;
	lsm6dso_dev_ctx.read_reg = i2c_master_read_LSM6DSO;
	lsm6dso_dev_ctx.i2c_number = SENSOR_BUS;

    /* Restore default configuration */
    lsm6dso_reset_set(&lsm6dso_dev_ctx, PROPERTY_ENABLE);
    do { lsm6dso_reset_get(&lsm6dso_dev_ctx, &LSM6DSO_rst); } while (LSM6DSO_rst);
    /* Disable I3C interface */
    lsm6dso_i3c_disable_set(&lsm6dso_dev_ctx, LSM6DSO_I3C_DISABLE);
    /* Enable Block Data Update */
    lsm6dso_block_data_update_set(&lsm6dso_dev_ctx, PROPERTY_ENABLE);
    /* Set Output Data Rate */
    lsm6dso_xl_data_rate_set(&lsm6dso_dev_ctx, LSM6DSO_XL_ODR_12Hz5);
    lsm6dso_gy_data_rate_set(&lsm6dso_dev_ctx, LSM6DSO_GY_ODR_12Hz5);
    /* Set full scale */
    lsm6dso_xl_full_scale_set(&lsm6dso_dev_ctx, LSM6DSO_2g);
    lsm6dso_gy_full_scale_set(&lsm6dso_dev_ctx, LSM6DSO_2000dps);
    /* Configure filtering chain(No aux interface) */
    /* Accelerometer - LPF1 + LPF2 path */
    lsm6dso_xl_hp_path_on_out_set(&lsm6dso_dev_ctx, LSM6DSO_LP_ODR_DIV_100);
    lsm6dso_xl_filter_lp2_set(&lsm6dso_dev_ctx, PROPERTY_ENABLE);

    return ESP_OK;
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Initialize LIS2MDL sensor
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
esp_err_t lis2mdl_init(void) {
	lis2mdl_dev_ctx.write_reg = i2c_master_write_LIS2MDL;
	lis2mdl_dev_ctx.read_reg = i2c_master_read_LIS2MDL;
	lis2mdl_dev_ctx.i2c_number = SENSOR_BUS;

    /* Enable Block Data Update */
    lis2mdl_block_data_update_set(&lis2mdl_dev_ctx, PROPERTY_ENABLE);
	/* Set Output Data Rate */
	lis2mdl_data_rate_set(&lis2mdl_dev_ctx, LIS2MDL_ODR_10Hz);
	/* Set / Reset sensor mode */
	lis2mdl_set_rst_mode_set(&lis2mdl_dev_ctx, LIS2MDL_SENS_OFF_CANC_EVERY_ODR);
	/* Enable temperature compensation */
	lis2mdl_offset_temp_comp_set(&lis2mdl_dev_ctx, PROPERTY_ENABLE);
	/* Set device in continuous mode */
	lis2mdl_operating_mode_set(&lis2mdl_dev_ctx, LIS2MDL_CONTINUOUS_MODE);

	return ESP_OK;
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : WhoAmI check if sensor is detected on I2C bus
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
esp_err_t whoami_check(void) {
    static const char *TAG = "WhoAmI";
    ESP_LOGI(TAG, "Master check sensors ---");

    // LSM6DSO ID
    lsm6dso_device_id_get(&lsm6dso_dev_ctx, &LSM6DSO_whoamI);
    if (LSM6DSO_whoamI != LSM6DSO_ID) { ESP_LOGE(TAG, "LSM6DSO not find"); }
    else { printf("Product ID= %d \n", LSM6DSO_whoamI); }

    // LIS2MDL ID
    lis2mdl_device_id_get(&lis2mdl_dev_ctx, &LIS2MDL_whoamI);
    if (LIS2MDL_whoamI != LIS2MDL_ID) { ESP_LOGE(TAG, "LIS2MDL not find"); }
    else { printf("Product ID= %d \n", LIS2MDL_whoamI); }

    ESP_LOGI(TAG, "End of check ---");
    fflush(stdout);
    return ESP_OK;
}


/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Get values from LSM6DSO sensor
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void get_LSM6DSO() {
    static const char *TAG = "get_LSM6DSO";
    lsm6dso_device_id_get(&lsm6dso_dev_ctx, &LSM6DSO_whoamI);
    if (LSM6DSO_whoamI != LSM6DSO_ID) { ESP_LOGE(TAG, "LSM6DSO not find"); }
    else {
        /* Read output only if new value is available */
        lsm6dso_reg_t reg;
        lsm6dso_status_reg_get(&lsm6dso_dev_ctx, &reg.status_reg);

        if (reg.status_reg.xlda) {
            memset(LSM6DSO_raw_acceleration, 0x00, 3 * sizeof(int16_t));
            lsm6dso_acceleration_raw_get(&lsm6dso_dev_ctx, LSM6DSO_raw_acceleration);
            LSM6DSO_acceleration_mg[0] = lsm6dso_from_fs2_to_mg(LSM6DSO_raw_acceleration[0]);
            LSM6DSO_acceleration_mg[1] = lsm6dso_from_fs2_to_mg(LSM6DSO_raw_acceleration[1]);
            LSM6DSO_acceleration_mg[2] = lsm6dso_from_fs2_to_mg(LSM6DSO_raw_acceleration[2]);

            lv_chart_set_next_value(ui_AccelerometerChart, ui_AccelerometerChart_series_1, LSM6DSO_acceleration_mg[0]);
            lv_chart_set_next_value(ui_AccelerometerChart, ui_AccelerometerChart_series_2, LSM6DSO_acceleration_mg[1]);
            lv_chart_set_next_value(ui_AccelerometerChart, ui_AccelerometerChart_series_3, LSM6DSO_acceleration_mg[2]);

            accel_x_update_value(&LSM6DSO_acceleration_mg[0]);
            accel_y_update_value(&LSM6DSO_acceleration_mg[1]);
            accel_z_update_value(&LSM6DSO_acceleration_mg[2]);

            /* update_sensor_chart("accelerometer", 1, &LSM6DSO_acceleration_mg[0]);
            update_sensor_chart("accelerometer", 2, &LSM6DSO_acceleration_mg[1]);
            update_sensor_chart("accelerometer", 3, &LSM6DSO_acceleration_mg[2]); */

            log_acceleration_X[acc_index] = LSM6DSO_acceleration_mg[0];
            log_acceleration_Y[acc_index] = LSM6DSO_acceleration_mg[1];
            log_acceleration_Z[acc_index] = LSM6DSO_acceleration_mg[2];
            acc_index++;
            if (acc_index == 100) {
                acc_index = 0;
            }

            sprintf((char *)tx_buffer, "Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
                    LSM6DSO_acceleration_mg[0], LSM6DSO_acceleration_mg[1], LSM6DSO_acceleration_mg[2]);
            printf((char const *)tx_buffer);
        }

        if (reg.status_reg.gda) {
            memset(LSM6DSO_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
            lsm6dso_angular_rate_raw_get(&lsm6dso_dev_ctx, LSM6DSO_raw_angular_rate);
            LSM6DSO_angular_rate_mdps[0] = lsm6dso_from_fs2000_to_mdps(LSM6DSO_raw_angular_rate[0]);
            LSM6DSO_angular_rate_mdps[1] = lsm6dso_from_fs2000_to_mdps(LSM6DSO_raw_angular_rate[1]);
            LSM6DSO_angular_rate_mdps[2] = lsm6dso_from_fs2000_to_mdps(LSM6DSO_raw_angular_rate[2]);

            lv_chart_set_next_value(ui_GyroChart, ui_GyroChart_series_1, LSM6DSO_angular_rate_mdps[0] / 1000.0f);
            lv_chart_set_next_value(ui_GyroChart, ui_GyroChart_series_2, LSM6DSO_angular_rate_mdps[1] / 1000.0f);
            lv_chart_set_next_value(ui_GyroChart, ui_GyroChart_series_3, LSM6DSO_angular_rate_mdps[2] / 1000.0f);

            gyro_x_update_value(&LSM6DSO_angular_rate_mdps[0]);
            gyro_y_update_value(&LSM6DSO_angular_rate_mdps[1]);
            gyro_z_update_value(&LSM6DSO_angular_rate_mdps[2]);

            /* update_sensor_chart("gyro", 1, &LSM6DSO_angular_rate_mdps[0]);
            update_sensor_chart("gyro", 2, &LSM6DSO_angular_rate_mdps[1]);
            update_sensor_chart("gyro", 3, &LSM6DSO_angular_rate_mdps[2]); */

            log_angular_rate_X[gyro_index] = LSM6DSO_angular_rate_mdps[0];
            log_angular_rate_Y[gyro_index] = LSM6DSO_angular_rate_mdps[1];
            log_angular_rate_Z[gyro_index] = LSM6DSO_angular_rate_mdps[2];
            gyro_index++;
            if (gyro_index == 100) {
                gyro_index = 0;
            }

            sprintf((char *)tx_buffer, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
                    LSM6DSO_angular_rate_mdps[0], LSM6DSO_angular_rate_mdps[1], LSM6DSO_angular_rate_mdps[2]);
            printf((char const *)tx_buffer);
        }

        if (reg.status_reg.tda) {
            memset(&LSM6DSO_raw_temperature, 0x00, sizeof(int16_t));
            lsm6dso_temperature_raw_get(&lsm6dso_dev_ctx, &LSM6DSO_raw_temperature);
            LSM6DSO_temperature_degC = lsm6dso_from_lsb_to_celsius(LSM6DSO_raw_temperature);
            
            lv_chart_set_next_value(ui_TempChart, ui_TempChart_series_1, LSM6DSO_temperature_degC);
            // update_sensor_chart("temp", 1, &LSM6DSO_temperature_degC);

            temp_update_value(&LSM6DSO_temperature_degC);

            log_temperature[temp_index] = LSM6DSO_temperature_degC;
            temp_index++;
            if (temp_index == 100) {
                temp_index = 0;
            }
            
            sprintf((char *)tx_buffer, "Temperature [degC]:%6.2f\r\n", LSM6DSO_temperature_degC);
            printf((char const *)tx_buffer);
        }
    }
}

/* --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
 * Function : Get values from LIS2MDL sensor
 * --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.-- */
void get_LIS2MDL() {
    static const char *TAG = "get_LIS2MDL";
    lis2mdl_device_id_get(&lis2mdl_dev_ctx, &LIS2MDL_whoamI);
    if (LIS2MDL_whoamI != LIS2MDL_ID) { ESP_LOGE(TAG, "LIS2MDL not find"); }
    else {
        lis2mdl_reg_t reg;
        lis2mdl_status_get(&lis2mdl_dev_ctx, &reg.status_reg);

        if (reg.status_reg.zyxda) {
            memset(LIS2MDL_raw_magnetic, 0x00, 3 * sizeof(int16_t));
            lis2mdl_magnetic_raw_get(&lis2mdl_dev_ctx, LIS2MDL_raw_magnetic);
            LIS2MDL_magnetic_mG[0] = lis2mdl_from_lsb_to_mgauss(LIS2MDL_raw_magnetic[0]);
            LIS2MDL_magnetic_mG[1] = lis2mdl_from_lsb_to_mgauss(LIS2MDL_raw_magnetic[1]);
            LIS2MDL_magnetic_mG[2] = lis2mdl_from_lsb_to_mgauss(LIS2MDL_raw_magnetic[2]);
            
            lv_chart_set_next_value(ui_MagnetometerChart, ui_MagnetometerChart_series_1, LIS2MDL_magnetic_mG[0] / 1000.0f);
            lv_chart_set_next_value(ui_MagnetometerChart, ui_MagnetometerChart_series_2, LIS2MDL_magnetic_mG[1]/ 1000.0f);
            lv_chart_set_next_value(ui_MagnetometerChart, ui_MagnetometerChart_series_3, LIS2MDL_magnetic_mG[2] / 1000.0f);

            mag_x_update_value(&LIS2MDL_magnetic_mG[0]);
            mag_y_update_value(&LIS2MDL_magnetic_mG[1]);
            mag_z_update_value(&LIS2MDL_magnetic_mG[2]);

            /* update_sensor_chart("magnetometer", 1, &LIS2MDL_magnetic_mG[0]);
            update_sensor_chart("magnetometer", 2, &LIS2MDL_magnetic_mG[1]);
            update_sensor_chart("magnetometer", 3, &LIS2MDL_magnetic_mG[2]); */

            log_magnetic_X[mag_index] = LIS2MDL_magnetic_mG[0];
            log_magnetic_Y[mag_index] = LIS2MDL_magnetic_mG[1];
            log_magnetic_Z[mag_index] = LIS2MDL_magnetic_mG[2];
            mag_index++;
            if (mag_index == 100) {
                mag_index = 0;
            }

            sprintf((char *)tx_buffer, "Magnetic field [mG]:%4.2f\t%4.2f\t%4.2f\r\n",
                    LIS2MDL_magnetic_mG[0], LIS2MDL_magnetic_mG[1], LIS2MDL_magnetic_mG[2]);
            printf((char const *)tx_buffer);
        }
    }
}