// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once

/*!
 * \file lora_esp32_config.h
 * \brief ESP32 lora configuration
 */

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "freertos/task.h"
#include "mcu_spi_config.h"
#include "rom/gpio.h"
#include "sdkconfig.h"
#include "soc/gpio_struct.h"

#define MCU_LORA_DEFAULT_CONFIG()                                       \
  {                                                                     \
    ._spi_transmit = _lora_SPI_transmit, ._delay = _lora_delay,         \
    ._gpio_set_level = _lora_GPIO_set_level, .log = _lora_log,          \
    .rst_gpio_num = CONFIG_LORA_RS, .cs_gpio_num = CONFIG_LORA_CS,      \
    .d0_gpio_num = CONFIG_LORA_D0, .implicit_header = 0, .frequency = 0 \
  }

/*!
 * \brief Mount spi config in lora config
 * \param[in] _spi_config spi config
 */
void lora_esp32_config_mount_spi_config(mcu_spi_config_t* _spi_config);

/*!
 * \brief Deinitialize lora config
 */
esp_err_t lora_esp32_config_deinit(void);

/*!
 * \brief GPIO initialization for LoRa
 * \param[in] _rst_gpio_num reset gpio number
 * \param[in] _cs_gpio_num chip select gpio number
 * \param[in] _d0_gpio_num DIO0 gpio number - for interrupts
 */
esp_err_t _lora_spi_and_pins_init(gpio_num_t _rst_gpio_num,
                                  gpio_num_t _cs_gpio_num,
                                  gpio_num_t _d0_gpio_num);

/*!
 * \brief SPI transmit function for LoRa
 * \param[in] _in input buffer
 * \param[out] _out output buffer
 */
bool _lora_SPI_transmit(uint8_t _in[2], uint8_t _out[2]);

/*!
 * \brief Delay function for LoRa
 * \param[in] _ms delay in milliseconds
 */

void _lora_delay(size_t _ms);

/*!
 * \brief GPIO set level function for LoRa
 * \param[in] _gpio_num gpio number
 * \param[in] _level level to set
 */
bool _lora_GPIO_set_level(uint8_t _gpio_num, uint32_t _level);

/*!
 * \brief Log function for LoRa
 * \param[in] info information to log
 */
void _lora_log(const char* info);
