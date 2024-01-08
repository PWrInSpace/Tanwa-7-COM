// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_spi_config.h"

esp_err_t spi_init(mcu_spi_config_t *spi_config) {
  esp_err_t ret = ESP_OK;
  if (spi_config->spi_init_flag) {
    return ESP_OK;
  }

  ret = spi_bus_initialize(spi_config->host_id, &spi_config->bus_config,
                           SPI_DMA_CH_AUTO);
  ESP_ERROR_CHECK(ret);

  ret = spi_bus_add_device(spi_config->host_id, &spi_config->dev_config,
                           &spi_config->spi_handle);
  ESP_ERROR_CHECK(ret);
  spi_config->spi_init_flag = true;
  return ret;
}

esp_err_t spi_deinit(mcu_spi_config_t *spi_config) {
  esp_err_t ret = ESP_OK;
  if (!spi_config->spi_init_flag) {
    return ESP_OK;
  }
  ret = spi_bus_remove_device(spi_config->spi_handle);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_free(spi_config->host_id);
  ESP_ERROR_CHECK(ret);
  spi_config->spi_init_flag = false;
  return ret;
}
