///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 25.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "app_init_task.h"

#include "esp_log.h"

#include "TANWA_config.h"

#define TAG "APP_INIT_TASK"

// static sdmmc_card_t sdmmc_card

// static sd_card_t sd_card = SD_CARD_DEFAULT_CONFIG(sdmmc_card);

// static sd_card_config_t sd_card_conf = SD_CARD_CONFIG_DEFAULT_CONFIG();

void app_init_task(void* pvParameters) {
  ESP_LOGI(TAG, "Initializing devices");
  esp_err_t ret = ESP_OK;

  ret |= TANWA_mcu_config_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "MCU configuration failed");
  }
  
  ret |= TANWA_hardware_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Hardware initialization failed");
  }

  ret |= TANWA_utility_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Utility initialization failed");
  }

  // for (int i = 0; i < 3; i++) {
  //   // ESP_LOGI(TAG, "%d", &devices_config->voltage_measure.adc_chan[i]);
  // }

  // xTaskCreatePinnedToCore(user_interface_task, "user_interface_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
  // xTaskCreatePinnedToCore(slave_com_task, "slave_com_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
  // xTaskCreatePinnedToCore(lora_task, "lora_task", 4096, (void*)&devices_config,
  //                         1, NULL, 1);
  // xTaskCreatePinnedToCore(memory_task, "mcu_memory_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
}
