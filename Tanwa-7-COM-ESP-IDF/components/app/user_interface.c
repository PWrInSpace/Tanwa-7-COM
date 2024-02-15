// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "user_interface.h"

#define USER_INTERFACE_TAG "USER_INTERFACE"

void user_interface_task(void* pvParameters) {
  TANWA_devices_t* devices_config = (TANWA_devices_t*)pvParameters;

  // buzzer_update_duty_cycle(&devices_config->buzzer, BUZZER_MAX_DUTY / 2);
  // vTaskDelay(pdMS_TO_TICKS(1000));
  // buzzer_update_duty_cycle(&devices_config->buzzer, 0);
  // vTaskDelay(pdMS_TO_TICKS(1000));
  // buzzer_update_duty_cycle(&devices_config->buzzer, BUZZER_MAX_DUTY / 2);
  // vTaskDelay(pdMS_TO_TICKS(1000));
  // buzzer_update_duty_cycle(&devices_config->buzzer, 0);
  // for (;;) {
  //   ESP_LOGI(USER_INTERFACE_TAG, "VCAN: %f V",
  //            voltage_measure_read_voltage(&devices_config->voltage_measure, 0));
  //   ESP_LOGI(USER_INTERFACE_TAG, "VBAT: %f V",
  //            voltage_measure_read_voltage(&devices_config->voltage_measure, 1));
  //   ESP_LOGI(USER_INTERFACE_TAG, "%f",
  //            voltage_measure_read_voltage(&devices_config->voltage_measure, 2));
  //   vTaskDelay(pdMS_TO_TICKS(1000));
  // }
}
