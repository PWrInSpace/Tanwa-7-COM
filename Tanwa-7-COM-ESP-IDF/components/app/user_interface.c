// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "user_interface.h"

#define USER_INTERFACE_TAG "USER_INTERFACE"

void user_interface_task(void* pvParameters) {
  ROSALIA_devices_t* devices_config = (ROSALIA_devices_t*)pvParameters;

  int center, top, bottom;
  ESP_LOGI(USER_INTERFACE_TAG, "RGB LED driver initialized");
  led_state_t toggle = LED_ON;
  rgb_led_toggle(&devices_config->rgb_led, toggle);
  buzzer_update_duty_cycle(&devices_config->buzzer, BUZZER_MAX_DUTY / 2);
  vTaskDelay(pdMS_TO_TICKS(1000));
  buzzer_update_duty_cycle(&devices_config->buzzer, 0);
  vTaskDelay(pdMS_TO_TICKS(1000));
  buzzer_update_duty_cycle(&devices_config->buzzer, BUZZER_MAX_DUTY / 2);
  vTaskDelay(pdMS_TO_TICKS(1000));
  buzzer_update_duty_cycle(&devices_config->buzzer, 0);
  for (;;) {
    rgb_led_update_duty_cycle(&devices_config->rgb_led,
                              (uint32_t[]){RGB_LED_MAX_DUTY, 0, 0});
    vTaskDelay(pdMS_TO_TICKS(1000));
    rgb_led_update_duty_cycle(&devices_config->rgb_led,
                              (uint32_t[]){0, RGB_LED_MAX_DUTY, 0});
    vTaskDelay(pdMS_TO_TICKS(1000));
    rgb_led_update_duty_cycle(&devices_config->rgb_led,
                              (uint32_t[]){0, 0, RGB_LED_MAX_DUTY});

    ESP_LOGI(USER_INTERFACE_TAG, "VCAN: %f V",
             voltage_measure_read_voltage(&devices_config->voltage_measure, 0));
    ESP_LOGI(USER_INTERFACE_TAG, "VBAT: %f V",
             voltage_measure_read_voltage(&devices_config->voltage_measure, 1));
    ESP_LOGI(USER_INTERFACE_TAG, "%f",
             voltage_measure_read_voltage(&devices_config->voltage_measure, 2));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
