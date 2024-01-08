// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#pragma once

#include "devices_config.h"

/*!
 * \brief LoRa sender task - sends data from UART to LoRa
 * \param[in] pvParameters Used to pass the ROSALIA devices configuration
 */
void lora_task(void* pvParameters);
