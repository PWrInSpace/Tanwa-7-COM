// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "app_init_task.h"

#define TAG "APP_INIT_TASK"

#define I2C_MASTER_FREQ_HZ 400000  // TODO(Glibus): sdkconig.h should have it

static adc_oneshot_unit_handle_t adc1_handle;

static sdmmc_card_t sdmmc_card;

static sd_card_t sd_card = SD_CARD_DEFAULT_CONFIG(sdmmc_card);

static sd_card_config_t sd_card_conf = SD_CARD_CONFIG_DEFAULT_CONFIG();

static TANWA_devices_t devices_config = {
    .spi = MCU_SPI_DEFAULT_CONFIG(),
    .i2c = MCU_I2C_DEFAULT_CONFIG(),
    .twai = MCU_TWAI_DEFAULT_CONFIG(),
    .voltage_measure = MCU_VOLTAGE_MEASURE_DEFAULT_CONFIG(),
    .lora = MCU_LORA_DEFAULT_CONFIG(),
    .buzzer = MCU_BUZZER_DRIVER_DEFAULT_CONFIG(),
    .memory =
        {
            .sd_card = &sd_card,
            .sd_card_config = &sd_card_conf,
        },
};

void app_init_task(void* pvParameters) {
  ESP_LOGI(TAG, "Initializing devices");

  devices_config.voltage_measure.oneshot_unit_handle = &adc1_handle;

  esp_err_t ret = devices_init(&devices_config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Devices initialization failed");
    vTaskDelete(NULL);
  }

  for (int i = 0; i < 3; i++) {
    // ESP_LOGI(TAG, "%d", &devices_config->voltage_measure.adc_chan[i]);
  }

  xTaskCreatePinnedToCore(user_interface_task, "user_interface_task", 4096,
                          (void*)&devices_config, 1, NULL, 1);
  // xTaskCreatePinnedToCore(slave_com_task, "slave_com_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
  xTaskCreatePinnedToCore(lora_task, "lora_task", 4096, (void*)&devices_config,
                          1, NULL, 1);
  // xTaskCreatePinnedToCore(memory_task, "mcu_memory_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
}
