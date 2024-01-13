// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include "flash_api.h"
#include "flash_nvs.h"
#include "mcu_spi_config.h"
#include "sdcard.h"
#include "sdkconfig.h"

#define SD_CARD_DEFAULT_CONFIG(sdmmc_card)                     \
  {                                                            \
    .card = &sdmmc_card, .spi_host = SPI2_HOST,                \
    .mount_point = SDCARD_MOUNT_POINT, .cs_pin = CONFIG_SD_CS, \
  }
  //.card_detect_pin = SD, .mounted = false,
  //}

#define SD_CARD_CONFIG_DEFAULT_CONFIG()                       \
  {                                                           \
    .spi_host = SPI2_HOST, .mount_point = SDCARD_MOUNT_POINT, \
    .cs_pin = CONFIG_SD_CS,                                   \
  }
  //.cd_pin = CONFIG_SD_CD,
  //}

typedef struct {
  sd_card_config_t* sd_card_config;
  sd_card_t* sd_card;
} mcu_memory_config_t;

/*!
 * \brief Initialize memory
 * \param[in] memory_config memory config
 * \returns ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t memory_init(mcu_memory_config_t* memory_config);

/*!
 * \brief Deinitialize memory
 * \param[in] memory_config memory config
 * \returns ESP_OK if successful, ESP_FAIL otherwise
 */
esp_err_t memory_deinit(mcu_memory_config_t* memory_config);

/*!
 * \brief Mounts SPI config to memory config
 * \param[in] memory_config memory config
 * \param[in] spi_config spi config
 */
void memory_config_mount_spi_config(mcu_memory_config_t* memory_config,
                                    mcu_spi_config_t* mcu_spi_config);
