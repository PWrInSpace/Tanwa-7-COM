// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include "devices_config.h"
#include "lora_task.h"
#include "slave_com.h"
#include "user_interface.h"
#include "memory_task.h"

/*!
 * \brief Task for initializing all the devices
 * \param pvParameters Parameters for task - this needs to be pointer to the
 * devices_config structure
 */
void app_init_task(void* pvParameters);
