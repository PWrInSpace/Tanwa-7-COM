// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

typedef struct {
  ledc_mode_t ledc_mode;
  uint8_t gpio_num;
  uint8_t ledc_channel_num;
  uint8_t ledc_timer_num;
  uint16_t duty;
  uint16_t max_duty;
} buzzer_driver_t;

/*!
 * \brief Initialize buzzer driver
 * \param config buzzer configuration
 */
esp_err_t buzzer_init(buzzer_driver_t *config, ledc_timer_bit_t ledc_duty_res,
                      uint32_t ledc_freq);

/*!
 * \brief Update buzzer duty cycle
 * \param config buzzer configuration
 * \param duty duty cycle
 */
esp_err_t buzzer_update_duty_cycle(buzzer_driver_t *config, uint16_t duty);
