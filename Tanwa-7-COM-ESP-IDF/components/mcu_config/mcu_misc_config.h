// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "rom/gpio.h"
#include "soc/gpio_struct.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "buzzer_driver.h"
#include "led_driver.h"

#define BUZZER_FREQ_HZ 4000
#define BUZZER_DUTY_RES LEDC_TIMER_13_BIT
#define BUZZER_TIMER LEDC_TIMER_1
#define BUZZER_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_MAX_DUTY 8191  // 2**13 - 1

#define MCU_BUZZER_DRIVER_DEFAULT_CONFIG()                              \
  {                                                                     \
    .ledc_mode = BUZZER_MODE, .gpio_num = CONFIG_BUZZER,                \
    .ledc_channel_num = LEDC_CHANNEL_3, .ledc_timer_num = BUZZER_TIMER, \
    .duty = 0, .max_duty = BUZZER_MAX_DUTY,                             \
  }

/*!
 * \brief Initialize misc drivers
 * \param buzzer buzzer driver
 */
esp_err_t misc_init(buzzer_driver_t *buzzer);

void _delay_ms(uint32_t ms);