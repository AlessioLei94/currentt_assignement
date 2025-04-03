#ifndef SHT40_H_
#define SHT40_H_

#include "driver/i2c_master.h"

struct sht40_handle_t {
    i2c_master_dev_handle_t i2c_dev;
    uint8_t buffer[6];
};

/**
 * @brief Get high precision measurement from sensor.
 * 
 * @param[in] handle: pointer to sht40 handle struct
 * @param[in] temp: pointer to variable in which temperature reading will be stored
 * @param[in] hum: pointer to variable in which humidity reading will be stored
 * @returns : 0 on success, negative error code otherwise
 */
esp_err_t sht40_get_measurement(struct sht40_handle_t *handle, int *temp, int *hum);

/**
 * @brief Read sensor serial number.
 * 
 * @param[in] handle: pointer to sht40 handle struct
 * @param[in] snr: pointer to variable in which serial number will be stored
 * @returns : 0 on success, negative error code otherwise
 */
esp_err_t sht40_read_snr(struct sht40_handle_t *handle, uint32_t *snr);

/**
 * @brief Initialize sensor.
 * 
 * @param[in] bus_handle: i2c masert bus handle
 * @param[in] dev_handle: pointer to sht40 handle struct, used to store i2c device handle
 * @returns : 0 on success, negative error code otherwise
 */
esp_err_t sht40_init(i2c_master_bus_handle_t bus_handle, struct sht40_handle_t *dev_handle);

#endif // SHT40_H_
