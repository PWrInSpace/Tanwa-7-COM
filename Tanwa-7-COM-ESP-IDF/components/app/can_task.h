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

/**
 * @brief Function for starting the can bus task.
 */
void run_can_task(void);

/**
 * @brief Function for stopping the can bus task.
 */
void stop_can_task(void);

/**
 * @brief Function for changing the period of the can bus task.
 * @param period_ms New period in milliseconds.
 */
void change_can_task_period(uint32_t period_ms);

/**
 * @brief Function for add a message to the counter.
 */
void can_task_add_rx_counter(void);

/**
 * @brief Function for subtract a message from the counter.
 */
void can_task_sub_rx_counter(void);

/**
 * @brief Task for receiving and parsing the CAN BUS messages.
 */
void can_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_CAN_TASK_H_ */