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


void run_lora_task(void);

/**
 * \brief LoRa sender task - sends data from UART to LoRa
 * \param[in] pvParameters Used to pass the devices configuration
 */
void lora_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_LORA_TASK_H_ */