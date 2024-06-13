///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the LoRa task. This task is for the LoRa communication.
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_LORA_TASK_H_
#define PWRINSPACE_TANWA_LORA_TASK_H_

#include <stdint.h>

/**
 * @brief Function for starting the LoRa task.
 */
void run_lora_task(void);

/**
 * @brief Function for stopping the LoRa task.
 */
void stop_lora_task(void);

/**
 * @brief Function for changing the period of the LoRa task.
 * @param period New period in milliseconds.
 */
void change_lora_task_period(uint32_t period_ms);

/**
 * \brief LoRa sender task - sends data from UART to LoRa
 * \param[in] pvParameters Used to pass the devices configuration
 */
void lora_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_LORA_TASK_H_ */