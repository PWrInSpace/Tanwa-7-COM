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
#ifndef PWRINSPACE_TANWA_MEASURE_TASK_H_
#define PWRINSPACE_TANWA_MEASURE_TASK_H_

#include <stdint.h>

/**
 * @brief Function for starting the measurement task.
 */
void run_measure_task(void);

/**
 * @brief Function for changing the period of the measurement task.
 * @param period Period of the task in ms.
 */
void change_measure_task_period(uint32_t period);

/**
 * @brief Task for measuring the temperature and pressure.
 */
void measure_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_MEASURE_TASK_H_ */
