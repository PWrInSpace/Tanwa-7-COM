// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "rom/gpio.h"
#include "sdkconfig.h"
#include "soc/gpio_struct.h"

#define MCU_SPI_DEFAULT_CONFIG()                   \
  {                                                \
    .host_id = SPI2_HOST,                          \
    .bus_config = {.miso_io_num = CONFIG_SPI_MISO, \
                   .mosi_io_num = CONFIG_SPI_MOSI, \
                   .sclk_io_num = CONFIG_SPI_SCK,  \
                   .quadwp_io_num = -1,            \
                   .quadhd_io_num = -1,            \
                   .max_transfer_sz = 4000},       \
    .dev_config = {.clock_speed_hz = 9000000,      \
                   .mode = 0,                      \
                   .spics_io_num = -1,             \
                   .queue_size = 1,                \
                   .flags = 0,                     \
                   .pre_cb = NULL},                \
    .spi_init_flag = false,                        \
  }

typedef struct {
  spi_host_device_t host_id;
  spi_device_handle_t spi_handle;
  spi_bus_config_t bus_config;
  spi_device_interface_config_t dev_config;
  bool spi_init_flag;
} mcu_spi_config_t;

/*!
 * \brief Initiates the SPI bus
 * \param spi_config SPI configuration
 * \return ESP_OK on success, ESP_FAIL otherwise
 * \note This function will only initiate SPI peripheral,
 *      it will not configure the GPIO for CS pins.
 */
esp_err_t spi_init(mcu_spi_config_t *spi_config);

/*!
 * \brief Deinitializes the SPI bus
 * \param[in] spi_config SPI configuration
 * \return ESP_OK on success, ESP_FAIL otherwise
*/
esp_err_t spi_deinit(mcu_spi_config_t *spi_config);
