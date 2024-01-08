// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

/*!
 * \file mcu_twai_config.h
 * \brief TWAI configuration and tools
 */

#include <stdint.h>

#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define MCU_TWAI_DEFAULT_CONFIG()                                     \
  {                                                                   \
    .tx_gpio_num = CONFIG_UART_1_RX, .rx_gpio_num = CONFIG_UART_1_TX, \
    .mode = TWAI_MODE_NORMAL                                          \
  }

/*!
 * \brief TWAI configuration structure
 * \param tx_gpio_num GPIO number for TX pin
 * \param rx_gpio_num GPIO number for RX pin
 * \param mode TWAI mode
 */
typedef struct {
  gpio_num_t tx_gpio_num;
  gpio_num_t rx_gpio_num;
  twai_mode_t mode;
} mcu_twai_config_t;

/*!
 * \brief TWAI initialization, initializes and starts TWAI driver
 * \note
 * \param config TWAI configuration structure
 * \return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t twai_init(mcu_twai_config_t *config, twai_general_config_t *g_config,
                    twai_timing_config_t *t_config,
                    twai_filter_config_t *f_config);

/*!
 * \brief TWAI message composition for self test purposes
 * \param id Message ID
 * \param data_length_code Message data length code
 * \param data Pointer to message data
 */
twai_message_t twai_compose_self_test_message(uint32_t id,
                                              uint8_t data_length_code,
                                              uint8_t *data);
