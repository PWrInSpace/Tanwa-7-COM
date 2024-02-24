// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_misc_config.h"

#include "esp_log.h"

#define TAG "MCU_MISC_CONFIG"

void _delay_ms(uint32_t ms) {
  vTaskDelay(pdMS_TO_TICKS(ms));
}