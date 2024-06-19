///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 31.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the CAN BUS task. This task is responsible for handling the
/// communication over the CAN BUS, it receives the data from the TWAI driver receive queue and
/// parses the commands.
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_CAN_TASK_H_
#define PWRINSPACE_TANWA_CAN_TASK_H_

#include <stdint.h>

#include "mcu_twai_config.h"

/**
 * @brief Function for starting the can bus task.
 */
void run_can_task(void);

/**
 * @brief Function for stopping the can bus task.
 */
void stop_can_task(void);

/**
 * @brief Function for adding the CAN message to the queue
 * @param message Pointer to the message.
 */
bool can_task_add_message(twai_message_t* message);

/**
 * @brief Function for adding the CAN message to the queue, with adding the rx counter.
 * @param message Pointer to the message.
 */
bool can_task_add_message_with_rx(twai_message_t* message);

bool can_task_check_alerts_and_recover(void);

/**
 * @brief Task for receiving and parsing the CAN BUS messages.
 */
void can_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_CAN_TASK_H_ */