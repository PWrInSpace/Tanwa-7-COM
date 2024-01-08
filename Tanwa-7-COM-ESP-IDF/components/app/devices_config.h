// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include "esp_log.h"
#include "lora.h"
#include "lora_esp32_config.h"
#include "mcu_adc_config.h"
#include "mcu_i2c_config.h"
#include "mcu_memory_config.h"
#include "mcu_misc_config.h"
#include "mcu_spi_config.h"
#include "mcu_twai_config.h"
#include "sdkconfig.h"
#include "ssd1306_esp32_config.h"

typedef struct {
  mcu_spi_config_t spi;
  mcu_i2c_config_t i2c;
  mcu_twai_config_t twai;
  voltage_measure_config_t voltage_measure;
  lora_struct_t lora;
  ssd1306_t oled_display;
  rgb_led_driver_t rgb_led;
  buzzer_driver_t buzzer;
  mcu_memory_config_t memory;
} ROSALIA_devices_t;

/*!
 * \brief Initiates all devices
 * \param devices Devices configuration
 * \return ESP_OK on success, ESP_FAIL otherwise
 * \note This function will initiate all devices.
 */
esp_err_t devices_init(ROSALIA_devices_t *devices);
