///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 12.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the solenoid valve driver utility. It is used to control
/// solenoid valves via GPIO pins of the PCA9574 I2C GPIO expander.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_SOLENOID_VALVE_DRIVER_H_
#define PWRINSPACE_SOLENOID_VALVE_DRIVER_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "pca9574.h"

#define SOLENOID_DRIVER_VALVE_COUNT 3

#define SOLENOID_DRIVER_TANWA_CONFIG(X)             \
  {                                                 \
    .pca9574 = X,                                   \
    .valves = {                                     \
        {SOLENOID_DRIVER_VALVE_FILL,                \
         SOLENOID_DRIVER_VALVE_GPIO_PIN_FILL,       \
         SOLENOID_DRIVER_LED_GPIO_PIN_FILL,         \
         SOLENOID_DRIVER_VALVE_STATE_CLOSE},        \
        {SOLENOID_DRIVER_VALVE_DEPR,                \
         SOLENOID_DRIVER_VALVE_GPIO_PIN_DEPR,       \
         SOLENOID_DRIVER_LED_GPIO_PIN_DEPR,         \
         SOLENOID_DRIVER_VALVE_STATE_CLOSE},        \
        {SOLENOID_DRIVER_VALVE_ADD,                 \
         SOLENOID_DRIVER_VALVE_GPIO_PIN_ADD,        \
         SOLENOID_DRIVER_LED_GPIO_PIN_ADD,          \
         SOLENOID_DRIVER_VALVE_STATE_CLOSE},        \
    }                                               \
  }

typedef enum {
    SOLENOID_DRIVER_VALVE_FILL = 0,
    SOLENOID_DRIVER_VALVE_DEPR = 1,
    SOLENOID_DRIVER_VALVE_ADD = 2
} solenoid_driver_valve_t;

typedef enum {
    SOLENOID_DRIVER_VALVE_GPIO_PIN_FILL = 2,
    SOLENOID_DRIVER_LED_GPIO_PIN_FILL = 5,
    SOLENOID_DRIVER_VALVE_GPIO_PIN_DEPR = 1,
    SOLENOID_DRIVER_LED_GPIO_PIN_DEPR = 4,
    SOLENOID_DRIVER_VALVE_GPIO_PIN_ADD = 0,
    SOLENOID_DRIVER_LED_GPIO_PIN_ADD = 3
} solenoid_driver_valve_gpio_pin_t;

typedef enum {
    SOLENOID_DRIVER_VALVE_STATE_CLOSE = 0,
    SOLENOID_DRIVER_VALVE_STATE_OPEN = 1
} solenoid_driver_valve_state_t;

typedef enum {
    SOLENOID_DRIVER_OK = 0,
    SOLENOID_DRIVER_FAIL = 1,
    SOLENOID_DRIVER_WRITE_ERROR = 2
} solenoid_driver_status_t;

typedef struct {
    solenoid_driver_valve_t valve;
    solenoid_driver_valve_gpio_pin_t gpio_pin;
    solenoid_driver_valve_gpio_pin_t led_gpio_pin;
    solenoid_driver_valve_state_t state;
} solenoid_struct_t;

typedef struct {
    pca9574_struct_t *pca9574;
    solenoid_struct_t valves[SOLENOID_DRIVER_VALVE_COUNT];
} solenoid_driver_struct_t;

solenoid_driver_status_t solenoid_driver_init(solenoid_driver_struct_t *solenoid_driver);

solenoid_driver_status_t solenoid_driver_valve_open(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve);

solenoid_driver_status_t solenoid_driver_valve_close(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve);

solenoid_driver_status_t solenoid_driver_valve_toggle(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve);

solenoid_driver_status_t solenoid_driver_valve_get_state(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve, solenoid_driver_valve_state_t *state);

#endif /* PWRINSPACE_SOLENOID_VALVE_DRIVER_H_ */
