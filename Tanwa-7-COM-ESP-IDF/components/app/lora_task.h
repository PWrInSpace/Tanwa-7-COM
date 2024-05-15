///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the measurement task. This task is responsible for measuring
/// the temperature and pressure from the sensors, as well as fetch the data from CAN bus.
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