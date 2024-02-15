// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "devices_config.h"

#define TAG "DEVICES_CONFIG"

esp_err_t devices_init(TANWA_devices_t* devices) {
  esp_err_t ret = ESP_OK;
  // twai_general_config_t twai_g_config = TWAI_GENERAL_CONFIG_DEFAULT(
  //     devices->twai.tx_gpio_num, devices->twai.rx_gpio_num, TWAI_MODE_NORMAL);
  // twai_timing_config_t twai_t_config = TWAI_TIMING_CONFIG_500KBITS();
  // twai_filter_config_t twai_f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  // ret |= twai_init(&devices->twai, &twai_g_config, &twai_t_config, &twai_f_config);
  // if (ret != ESP_OK) {
  //   ESP_LOGE(TAG, "TWAI initialization failed");
  //   return ret;
  // }
  // ret |= spi_init(&devices->spi);
  // lora_esp32_config_mount_spi_config(&devices->spi);
  // ret = _lora_spi_and_pins_init(devices->lora.rst_gpio_num,
  //                               devices->lora.cs_gpio_num,
  //                               devices->lora.d0_gpio_num);

  // ret |= lora_init(&devices->lora);

  // vTaskDelay(pdMS_TO_TICKS(100));

  // lora_set_frequency(&devices->lora, 867e6);
  // lora_set_bandwidth(&devices->lora, LORA_BW_250_kHz);
  // lora_disable_crc(&devices->lora);

  // ret |= mcu_i2c_init();

  // misc_init(&devices->buzzer);
  // voltage_measure_init(&devices->voltage_measure);

  // memory_config_mount_spi_config(&devices->memory, &devices->spi);
  // memory_init(&devices->memory);

  vTaskDelay(3000 / portTICK_PERIOD_MS);

  return ret;
}
