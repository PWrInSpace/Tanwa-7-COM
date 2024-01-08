// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_i2c_config.h"

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
