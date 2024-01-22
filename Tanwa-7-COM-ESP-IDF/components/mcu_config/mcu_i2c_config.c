///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 22.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "mcu_i2c_config.h"
#include <string.h>

#define I2C_TAG "I2C_CONFIG"

esp_err_t i2c_init(mcu_i2c_config_t *i2c) {
  if (i2c->i2c_init_flag == false) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c->sda,
        .scl_io_num = i2c->scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c->clk_speed,
    };
    i2c_param_config(i2c->port, &conf);
    esp_err_t ret = i2c_driver_install(i2c->port, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
      ESP_LOGE(I2C_TAG, "I2C driver install error: %d", ret);
      return ret;
    }
    i2c->i2c_init_flag = true;
  }
  return ESP_OK;
}

bool _tmp1075_I2C_write_TS1(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    // dynamically create a buffer to hold the data to be written
    uint8_t *write_buf = malloc(len + 1);
    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);
    ret = i2c_master_write_to_device(CONFIG_I2C_MASTER_PORT_NUM, CONFIG_I2C_TMP1075_TS1_ADDR, write_buf, sizeof(write_buf), CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    free(write_buf);
    return (bool)(ret == ESP_OK);
}

bool _tmp1075_I2C_read_TS1(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(CONFIG_I2C_MASTER_PORT_NUM, CONFIG_I2C_TMP1075_TS1_ADDR, &reg, 1, data, len, CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (bool)(ret == ESP_OK);
}

bool _tmp1075_I2C_write_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    // dynamically create a buffer to hold the data to be written
    uint8_t *write_buf = malloc(len + 1);
    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);
    ret = i2c_master_write_to_device(CONFIG_I2C_MASTER_PORT_NUM, CONFIG_I2C_TMP1075_TS2_ADDR, write_buf, sizeof(write_buf), CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    free(write_buf);
    return (bool)(ret == ESP_OK);
}

bool _tmp1075_I2C_read_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(CONFIG_I2C_MASTER_PORT_NUM, CONFIG_I2C_TMP1075_TS2_ADDR, &reg, 1, data, len, CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (bool)(ret == ESP_OK);
}

bool _mcp23018_I2C_write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    // dynamically create a buffer to hold the data to be written
    uint8_t *write_buf = malloc(len + 1);
    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);
    ret = i2c_master_write_to_device(CONFIG_I2C_MASTER_PORT_NUM, CONFIG_I2C_MCP23018_ADDR, write_buf, sizeof(write_buf), CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    free(write_buf);
    return (bool)(ret == ESP_OK);
}

bool _mcp23018_I2C_read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(CONFIG_I2C_MASTER_PORT_NUM, CONFIG_I2C_MCP23018_ADDR, &reg, 1, data, len, CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (bool)(ret == ESP_OK);
}
