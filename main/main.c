#include "sht40.h"
#include "driver/i2c_master.h"

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#define TAG "main"

#define SCL_IO_PIN 9
#define SDA_IO_PIN 14
#define PORT_NUMBER -1 // Auto-select

void sensor_read_task(void *arg)
{
    esp_err_t rv = ESP_OK;
    struct sht40_handle_t *handle = arg;
    int temp = 0;
    int hum = 0;

    while (true) {
        rv = sht40_get_measurement(handle, &temp, &hum);
        if (rv != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get sensor readings, error %d", rv);
        } else {
            ESP_LOGI(TAG, "Temperature %d Humidity %d", temp, hum);
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

int app_main(void)
{
    esp_err_t rv = ESP_OK;
    i2c_master_bus_handle_t bus_handle;
    struct sht40_handle_t sht40_handle;

    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = PORT_NUMBER,
        .scl_io_num = SCL_IO_PIN,
        .sda_io_num = SDA_IO_PIN,
        .glitch_ignore_cnt = 7,
    };

    rv = i2c_new_master_bus(&i2c_bus_config, &bus_handle);
    if (rv != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init i2c master, error %d", rv);
        return rv;
    }

    rv = sht40_init(bus_handle, &sht40_handle);
    if (rv != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init SHT40 device, error %d", rv);
        return rv;
    }

    rv = xTaskCreate(sensor_read_task, "SHT40_task", 1024, &sht40_handle, 7, NULL);
    if (rv != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create SHT40 task, error %d", rv);
        return rv;
    }

    return rv;
}
