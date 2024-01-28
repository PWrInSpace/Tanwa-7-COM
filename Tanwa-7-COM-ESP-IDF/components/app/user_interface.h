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


void user_interface_task(void* pvParameters);
