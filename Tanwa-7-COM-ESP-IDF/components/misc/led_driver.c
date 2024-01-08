// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "led_driver.h"

#define LED_DRIVER_TAG "LED_Driver"

esp_err_t led_timer_init(led_driver_t *led_drv, ledc_timer_bit_t ledc_duty_res,
                         uint32_t ledc_freq) {
  ledc_timer_config_t ledc_timer = {.speed_mode = led_drv->ledc_mode,
                                    .timer_num = led_drv->ledc_timer_num,
                                    .duty_resolution = ledc_duty_res,
                                    .freq_hz = ledc_freq,
                                    .clk_cfg = LEDC_AUTO_CLK};
  if (ledc_timer_config(&ledc_timer) != ESP_OK) {
    ESP_LOGE(LED_DRIVER_TAG, "Ledc timer config failed!");
    return ESP_FAIL;
  }
  return ESP_OK;
}

esp_err_t led_driver_init(led_driver_t *led_drv) {
  ledc_channel_config_t ledc_channel = {.speed_mode = led_drv->ledc_mode,
                                        .channel = led_drv->ledc_channel_num,
                                        .timer_sel = led_drv->ledc_timer_num,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = led_drv->led_gpio_num,
                                        .duty = 0,  // Set duty to 0%
                                        .hpoint = 0};
  if (led_drv->inverted == true) {
    ledc_channel.flags.output_invert = 1;
  }
  if (ledc_channel_config(&ledc_channel) != ESP_OK) {
    ESP_LOGE(LED_DRIVER_TAG, "LEDc channel config failed");
    return ESP_FAIL;
  }

  if (led_toggle(led_drv, LED_ON) != ESP_OK) {
    ESP_LOGE(LED_DRIVER_TAG, "LED driver toggle failed");
    return ESP_FAIL;
  }

  ESP_LOGI(LED_DRIVER_TAG, "LED driver initialized");

  return ESP_OK;
}

esp_err_t led_update_duty_cycle(led_driver_t *led_drv, uint32_t duty) {
  if (duty > led_drv->max_duty) {
    ESP_LOGE(LED_DRIVER_TAG, "Duty too large, expected max. %d, actual: %d",
             led_drv->max_duty, duty);
    return ESP_FAIL;
  }
  if (ledc_set_duty(led_drv->ledc_mode, led_drv->ledc_channel_num, duty) !=
      ESP_OK) {
    ESP_LOGE(LED_DRIVER_TAG, "LEDc set duty failed");
    return ESP_FAIL;
  }

  if (ledc_update_duty(led_drv->ledc_mode, led_drv->ledc_channel_num) !=
      ESP_OK) {
    ESP_LOGE(LED_DRIVER_TAG, "LEDc Update duty failed");
    return ESP_FAIL;
  }

  led_drv->duty = duty;

  return ESP_OK;
}

esp_err_t led_toggle(led_driver_t *led_drv, led_state_t toggle) {
  if (toggle == led_drv->toggle) {
    ESP_LOGI(LED_DRIVER_TAG, "LED already in state %d", toggle);
    return ESP_OK;
  }
  switch (toggle) {
    case LED_OFF:
      if (led_update_duty_cycle(led_drv, 0) != ESP_OK) {
        ESP_LOGE(LED_DRIVER_TAG, "LED driver update duty cycle failed");
        return ESP_FAIL;
      }
      break;
    case LED_ON:
      if (led_update_duty_cycle(led_drv, led_drv->duty) != ESP_OK) {
        ESP_LOGE(LED_DRIVER_TAG, "LED driver update duty cycle failed");
        return ESP_FAIL;
      }
      break;
    default:
      ESP_LOGE(LED_DRIVER_TAG, "Unknown LED state %d", toggle);
      return ESP_FAIL;
  }
  led_drv->toggle = toggle;
  return ESP_OK;
}
