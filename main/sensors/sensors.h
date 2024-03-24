#ifndef SENSORS_H
#define SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t i2c_master_init(void);
int32_t i2c_master_write_LSM6DSO(uint8_t i2c_num, uint8_t regaddr, uint8_t
                                 *data_wr, uint16_t size);
int32_t i2c_master_read_LSM6DSO(uint8_t i2c_num, uint8_t regaddr, uint8_t
                                *data_rd, uint16_t size);
int32_t i2c_master_write_LIS2MDL(uint8_t i2c_num, uint8_t regaddr, uint8_t
                                 *data_wr, uint16_t size);
int32_t i2c_master_read_LIS2MDL(uint8_t i2c_num, uint8_t regaddr, uint8_t
                                *data_rd, uint16_t size);
esp_err_t lsm6dso_init(void);
esp_err_t lis2mdl_init(void);
esp_err_t whoami_check(void);
void get_LSM6DSO();
void get_LIS2MDL();

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* SENSORS_H */