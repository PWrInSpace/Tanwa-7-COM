///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the ESP-NOW task. This task is for the ESP-NOW communication.
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_ESP_NOW_TASK_H_
#define PWRINSPACE_TANWA_ESP_NOW_TASK_H_

#include <stdint.h>

/**
 * @brief Function for starting the ESP-NOW task.
 */
void run_esp_now_task(void);

/**
 * @brief Function for stopping the ESP-NOW task.
 */
void stop_esp_now_task(void);

/**
 * @brief Task for handling the ESP-NOW communication.
 */
void esp_now_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_ESP_NOW_TASK_H_ */