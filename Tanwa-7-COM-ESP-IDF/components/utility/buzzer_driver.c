// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "buzzer_driver.h"

#define BUZZER_TAG "BUZZER"

esp_err_t buzzer_init(buzzer_driver_t *config, ledc_timer_bit_t ledc_duty_res,
                      uint32_t ledc_freq) {
  ledc_timer_config_t ledc_timer = {.speed_mode = config->ledc_mode,
                                    .timer_num = config->ledc_timer_num,
                                    .duty_resolution = ledc_duty_res,
                                    .freq_hz = ledc_freq,
                                    .clk_cfg = LEDC_AUTO_CLK};
  if (ledc_timer_config(&ledc_timer) != ESP_OK) {
    ESP_LOGE(BUZZER_TAG, "Ledc timer config failed!");
    return ESP_FAIL;
  }

  ledc_channel_config_t ledc_channel = {.speed_mode = config->ledc_mode,
                                        .channel = config->ledc_channel_num,
                                        .timer_sel = config->ledc_timer_num,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = config->gpio_num,
                                        .duty = 0,  // Set duty to 0%
                                        .hpoint = 0};
  if (ledc_channel_config(&ledc_channel) != ESP_OK) {
    ESP_LOGE(BUZZER_TAG, "LEDc channel config failed");
    return ESP_FAIL;
  }
  return ESP_OK;
}

esp_err_t buzzer_update_duty_cycle(buzzer_driver_t *config, uint16_t duty) {
  if (duty > config->max_duty) {
    ESP_LOGE(BUZZER_TAG, "Duty too large, expected max. %d, actual: %d",
             config->max_duty, duty);
    return ESP_FAIL;
  }
  if (ledc_set_duty(config->ledc_mode, config->ledc_channel_num, duty) !=
      ESP_OK) {
    ESP_LOGE(BUZZER_TAG, "LEDc set duty failed");
    return ESP_FAIL;
  }

  if (ledc_update_duty(config->ledc_mode, config->ledc_channel_num) != ESP_OK) {
    ESP_LOGE(BUZZER_TAG, "LEDc Update duty failed");
    return ESP_FAIL;
  }
  return ESP_OK;
}
