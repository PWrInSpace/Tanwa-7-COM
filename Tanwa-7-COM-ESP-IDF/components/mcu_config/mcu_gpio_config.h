///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 22.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the configuration of the GPIO pins of the MCU.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_MCU_GPIO_CONFIG_H_
#define PWRINSPACE_MCU_GPIO_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "rom/gpio.h"
#include "soc/gpio_struct.h"

typedef enum {
    LED_GPIO = CONFIG_GPIO_LED,
    LORA_RST_GPIO = CONFIG_LORA_RS,
    LORA_CS_GPIO = CONFIG_LORA_CS,
    LORA_D0_GPIO = CONFIG_LORA_D0,
    ABORT_GPIO = CONFIG_GPIO_ABORT,
    BUZZER_GPIO = CONFIG_BUZZER,
    ARM_GPIO = CONFIG_ARM,
    FIRE_1_GPIO = CONFIG_IGNIT_FIRE_1,
    FIRE_2_GPIO = CONFIG_IGNIT_FIRE_2,
} mcu_gpio_cfg_t;

typedef enum {
    LED_GPIO_INDEX = 0,
    LORA_RST_GPIO_INDEX,
    LORA_CS_GPIO_INDEX,
    LORA_D0_GPIO_INDEX,
    ABORT_GPIO_INDEX,
    BUZZER_GPIO_INDEX,
    ARM_GPIO_INDEX,
    FIRE_1_GPIO_INDEX,
    FIRE_2_GPIO_INDEX,
    MAX_GPIO_INDEX
} mcu_gpio_index_cfg_t;

typedef struct {
    uint8_t pins[MAX_GPIO_INDEX];
    uint8_t num_pins;
    gpio_config_t configs[MAX_GPIO_INDEX];
} mcu_gpio_config_t;

esp_err_t mcu_gpio_init(void);

bool _mcu_gpio_set_level(uint8_t gpio, uint8_t level);

bool _mcu_gpio_get_level(uint8_t gpio, uint8_t* level);

bool _lora_gpio_set_level(uint8_t gpio, uint8_t level);

bool _lora_gpio_attach_d0_isr(gpio_isr_t interrupt_cb);

bool _abort_gpio_attach_isr(gpio_isr_t interrupt_cb);

#endif /* PWRINSPACE_MCU_GPIO_CONFIG_H_ */