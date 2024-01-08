// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define MCU_I2C_DEFAULT_CONFIG()                                     \
  {                                                                  \
    .port = I2C_NUM_0, .sda = CONFIG_I2C_SDA, .scl = CONFIG_I2C_SCL, \
    .clk_speed = I2C_MASTER_FREQ_HZ, .i2c_init_flag = false,         \
  }

typedef struct {
  i2c_port_t port;
  i2c_cmd_handle_t *cmd;
  gpio_num_t sda;
  gpio_num_t scl;
  uint32_t clk_speed;
  uint32_t timeout;
  bool i2c_init_flag;
} mcu_i2c_config_t;

/*!
 * \brief Initiates the I2C bus
 * \param i2c I2C configuration
 * \return ESP_OK on success, ESP_FAIL otherwise
 * \note This function will initiate I2C peripheral.
 */
esp_err_t i2c_init(mcu_i2c_config_t *i2c);
