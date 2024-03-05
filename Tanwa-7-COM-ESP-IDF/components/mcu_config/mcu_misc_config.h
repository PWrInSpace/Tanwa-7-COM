///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 29.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains wrappers for miscelanous functions and RTOS functions.
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_MCU_MISC_CONFIG_H_
#define PWRINSPACE_MCU_MISC_CONFIG_H_

#include <stdint.h>

void _LED_delay_ms(uint32_t ms);

void _IGNITER_delay_ms(uint32_t ms);

void _LORA_delay_ms(uint32_t ms);

#endif /* PWRINSPACE_MCU_MISC_CONFIG_H_ */