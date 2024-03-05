///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 29.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "mcu_misc_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "MCU_MISC_CONFIG"

void _LED_delay_ms(uint32_t ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}

void _IGNITER_delay_ms(uint32_t ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}

void _LORA_delay_ms(uint32_t ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}