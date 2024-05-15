///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 25.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "app_init_task.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "TANWA_config.h"
#include "mcu_adc_config.h"
#include "state_machine_config.h"

#include "console_config.h"

#include "measure_task.h"

#define TAG "APP_INIT_TASK"

// static sdmmc_card_t sdmmc_card;

// static sd_card_t sd_card = SD_CARD_DEFAULT_CONFIG(sdmmc_card);

// static sd_card_config_t sd_card_conf = SD_CARD_CONFIG_DEFAULT_CONFIG();

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

void app_init_task(void* pvParameters) {
  ESP_LOGI(TAG, "### App initialization task started ###");
  esp_err_t ret = ESP_OK;

  ESP_LOGI(TAG, "Initializing MCU configuration...");

  ret |= TANWA_mcu_config_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "MCU configuration failed");
  }

  ESP_LOGI(TAG, "Initializing hardware...");
  
  ret |= TANWA_hardware_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Hardware initialization failed");
  }

  ESP_LOGI(TAG, "Initializing utility...");

  ret |= TANWA_utility_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Utility initialization failed");
  }

  ESP_LOGI(TAG, "Initializing system...");

  if (!initialize_state_machine()) {
    ESP_LOGE(TAG, "State machine initialization failed");
  }

  ESP_LOGI(TAG, "### App initialization finished ###");

  ESP_LOGI(TAG, "Starting console...");

  ret |= console_config_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Console initialization failed");
  } else {
    ESP_LOGI(TAG, "Console initialized");
  }

  run_measure_task();

  // xTaskCreatePinnedToCore(user_interface_task, "user_interface_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
  // xTaskCreatePinnedToCore(slave_com_task, "slave_com_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
  // xTaskCreatePinnedToCore(lora_task, "lora_task", 4096, (void*)&devices_config,
  //                         1, NULL, 1);
  // xTaskCreatePinnedToCore(memory_task, "mcu_memory_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);

  // while (1) {
  //   int16_t raw;
  //   float temp, voltage, pressure;
  //   tmp1075_get_temp_raw(&(TANWA_hardware.tmp1075[0]), &raw);
  //   tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[0]), &temp);
  //   printf("#TS1=> raw: %d, temp: %f\n", raw, temp);
  //   tmp1075_get_temp_raw(&(TANWA_hardware.tmp1075[1]), &raw);
  //   tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[1]), &temp);
  //   printf("#TS2=> raw: %d, temp: %f\n", raw, temp);
  //   pressure_driver_read_voltage(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &voltage);
  //   pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure);
  //   printf("#ADC=> voltage: %f, pressure: %f\n", voltage, pressure);
  //   _mcu_adc_read_voltage(VBAT_CHANNEL_INDEX, &voltage);
  //   printf("#VBAT=> voltage: %f\n", voltage);
  //   if (TANWA_utility.led_state_display.state == LED_STATE_DISPLAY_STATE_IDLE) {
  //       led_state_display_state_update(&(TANWA_utility.led_state_display), LED_STATE_DISPLAY_STATE_ARMED);
  //   } else {
  //       led_state_display_state_update(&(TANWA_utility.led_state_display), LED_STATE_DISPLAY_STATE_IDLE);
  //   }
  //   vTaskDelay(2000 / portTICK_PERIOD_MS);
  //   solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
  //   solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
  //   solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
  //   led_toggle(&(TANWA_hardware.esp_led));
  //   vTaskDelay(500 / portTICK_PERIOD_MS);
  //   if (TANWA_hardware.igniter[0].state == IGNITER_STATE_WAITING) {
  //       igniter_continuity_t continuity;
  //       igniter_check_continuity(&(TANWA_hardware.igniter[0]), &continuity);
  //       igniter_arm(&(TANWA_hardware.igniter[0]));
  //       vTaskDelay(100 / portTICK_PERIOD_MS);
  //       igniter_fire(&(TANWA_hardware.igniter[0]));
  //   } else {
  //       igniter_disarm(&(TANWA_hardware.igniter[0]));
  //       igniter_reset(&(TANWA_hardware.igniter[0]));
  //   }
  //   vTaskDelay(500 / portTICK_PERIOD_MS);
  //   if (TANWA_hardware.igniter[1].state == IGNITER_STATE_WAITING) {
  //       igniter_continuity_t continuity;
  //       igniter_check_continuity(&(TANWA_hardware.igniter[1]), &continuity);
  //       igniter_arm(&(TANWA_hardware.igniter[1]));
  //       vTaskDelay(100 / portTICK_PERIOD_MS);
  //       igniter_fire(&(TANWA_hardware.igniter[1]));
  //   } else {
  //       igniter_disarm(&(TANWA_hardware.igniter[1]));
  //       igniter_reset(&(TANWA_hardware.igniter[1]));
  //   }
  // }
  vTaskDelete(NULL);
}

void run_app_init(void) {
  xTaskCreatePinnedToCore(app_init_task, "app_init_task", 4096, NULL, 1, NULL, 1);
}