#include "sht40.h"

#include "driver/i2c_master.h"

#include "esp_types.h"
#include "esp_log.h"
#define TAG "sht40"

#define SHT40_I2C_ADDR 0x44
#define SHT40_I2C_SPEED_HZ 100000
// SHT40 command bytes
#define SHT40_CMD_MEAS_HIGHP 0xFD
#define SHT40_CMD_READ_SNR 0x89

// Private APIs

static esp_err_t i2c_write(struct sht40_handle_t *handle, uint8_t address, const uint8_t *data, uint32_t size)
{
    handle->buffer[0] = address;
    for (int i=0; i < size; i++) {
        handle->buffer[i+1] = data[0];
    }
    return i2c_master_transmit(handle->i2c_dev, handle->buffer, size + 1, -1);
}

static esp_err_t i2c_read(struct sht40_handle_t *handle, uint8_t address, uint8_t *data, uint32_t size)
{
    handle->buffer[0] = address & 0x80; // set bit 7 to 1 to indicate we want to read
    return i2c_master_transmit_receive(handle->i2c_dev, handle->buffer, 1, data, size, -1);
}

// Public APIs

esp_err_t sht40_get_measurement(struct sht40_handle_t *handle, int *temp, int *hum)
{
    esp_err_t rv = ESP_OK;
    uint16_t temp_ticks = 0;
    uint16_t hum_ticks = 0;

    rv = i2c_read(handle, SHT40_CMD_MEAS_HIGHP, handle->buffer, sizeof(handle->buffer));
    if (rv != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get high precision measurement, error %d", rv);
        return rv;
    }

    temp_ticks = (handle->buffer[0] << 8) + handle->buffer[1];
    hum_ticks = (handle->buffer[3] << 8) + handle->buffer[4]; 

    // Check CRCs at bytes 2 and 5, if wrong return error

    // Convert ticks to values, based on sensor's datasheet
    *temp = -45 + 175 * (temp_ticks / 65535);
    *hum = -6 + 125 * (hum_ticks / 65535);

    if (*temp > 100) {
        *temp = 100;
    }

    if (*hum < 0) {
        *hum = 0;
    }

    return rv;
}

esp_err_t sht40_read_snr(struct sht40_handle_t *handle, uint32_t *snr)
{
    esp_err_t rv = ESP_OK;

    rv = i2c_read(handle, SHT40_CMD_READ_SNR, handle->buffer, sizeof(handle->buffer));
    if (rv != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read SNR, error %d", rv);
        return rv;
    }

    // Check CRCs at bytes 2 and 5, if wrong return error

    *snr = (((handle->buffer[0] << 8) + handle->buffer[1]) << 16) + ((handle->buffer[3] << 8) + handle->buffer[4]);

    return rv;
}

esp_err_t sht40_init(i2c_master_bus_handle_t bus_handle, struct sht40_handle_t *dev_handle)
{
    esp_err_t rv = ESP_OK;
    uint32_t snr = 0;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT40_I2C_ADDR,
        .scl_speed_hz = SHT40_I2C_SPEED_HZ,
    };

    rv = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle->i2c_dev);
    if (rv != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SHT40 device to i2c bus, error %d", rv);
        return rv;
    }

    rv =  sht40_read_snr(dev_handle, &snr);
    if (rv == ESP_OK) {
        ESP_LOGI(TAG, "Device SNR %ld", snr);
    }

    return rv;
}
