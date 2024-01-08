// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include "devices_config.h"
#include "esp_err.h"
#include "flash_api.h"
#include "flash_nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "sdcard.h"
#include "usb_msc_api.h"

/*!
 * \brief Memory task
 * \param[in] pvParameters Used to pass the ROSALIA devices configuration
 */
void memory_task(void* pvParameters);
