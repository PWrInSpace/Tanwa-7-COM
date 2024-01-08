// Copyright 2023 PWr in Space, Krzysztof Gliwiński
#pragma once

#include "devices_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mcu_adc_config.h"
#include "sdkconfig.h"

/*!
 * \file user_interface.h
 * \brief User interface app header file
 * Contains task for rgb LED, status LED, buzzer and button handling
 */

/*!
 * \brief RGB LED color enum
 */
typedef enum {
  NONE = 0b000,
  RED = 0b001,
  GREEN = 0b010,
  BLUE = 0b100,
  YELLOW = RED | GREEN,
  CYAN = GREEN | BLUE,
  MAGENTA = RED | BLUE,
  WHITE = RED | GREEN | BLUE
} rgb_led_color_t;

typedef enum {
  CAN_CHANNEL = ADC_CHANNEL_0,
  VBAT_CHANNEL = ADC_CHANNEL_1,
  ADJV_CHANNEL = ADC_CHANNEL_3,
} adc_chan_cfg_t;

typedef enum {
  CAN_CHANNEL_INDEX = 0,
  VBAT_CHANNEL_INDEX,
  ADJV_CHANNEL_INDEX,
  MAX_CHANNEL_INDEX
} adc_chan_index_cfg_t;

void user_interface_task(void* pvParameters);
