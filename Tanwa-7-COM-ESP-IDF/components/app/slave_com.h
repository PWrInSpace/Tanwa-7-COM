// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include "devices_config.h"
#include "freertos/FreeRTOS.h"

/*!
 * \brief Task for slave communication thtough TWAI or ESP-NOW
 * \param pvParameters Parameters for task - this needs to be pointer to the
 * devices_config structure
 */
void slave_com_task(void *pvParameters);
