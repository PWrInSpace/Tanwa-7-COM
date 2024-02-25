///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 25.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the system initialization task. This task is responsible for
/// initializing all the mcu configuration, all hardware devices and all the utilities.
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_APP_INIT_TASK_H_
#define PWRINSPACE_TANWA_APP_INIT_TASK_H_

/**
 * @brief Function for initializing the system.
 * 
 */
void run_app_init(void);

/**
 * @brief Task for initializing of the system.
 */
void app_init_task(void* pvParameters);

#endif /* PWRINSPACE_TANWA_APP_INIT_TASK_H_ */