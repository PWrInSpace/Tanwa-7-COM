// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_misc_config.h"

#define MISC_TAG "MCU_MISC_CONFIG"

esp_err_t misc_init(buzzer_driver_t *buzzer) {
  esp_err_t err = ESP_OK;
  err = buzzer_init(buzzer, BUZZER_DUTY_RES, BUZZER_FREQ_HZ);
  if (err != ESP_OK) {
    ESP_LOGE(MISC_TAG, "buzzer_init failed with error %d", err);
    return err;
  }
  return err;
}

esp_err_t led_GPIO_init(led_struct_t *led) {
  esp_err_t err = ESP_OK;
  if (led == NULL) {
    ESP_LOGE(MISC_TAG, "Invalid argument - NULL check failed");
    return ESP_FAIL;
  }
  // Configure gpio
  gpio_config_t gpio_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << led->gpio_num),
      .pull_down_en = 0,
      .pull_up_en = 0,
  };
  if (led->drive == LED_DRIVE_OPEN_DRAIN) {
    gpio_conf.mode = GPIO_MODE_OUTPUT_OD;
  }
  err = gpio_config(&gpio_conf);
  if (err != ESP_OK) {
    ESP_LOGE(MISC_TAG, "GPIO config for LED failed with error %d", err);
    return err;
  }
  return err;
}

bool _GPIO_set_level(uint8_t gpio_num, uint8_t level) {
  return gpio_set_level(gpio_num, level) == ESP_OK ? true : false;
}

void _delay_ms(uint32_t ms) { 
  vTaskDelay(pdMS_TO_TICKS(ms));
}