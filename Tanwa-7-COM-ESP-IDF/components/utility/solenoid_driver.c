///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 12.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "solenoid_driver.h"

solenoid_driver_status_t solenoid_driver_init(solenoid_driver_struct_t *solenoid_driver) {
    if (solenoid_driver == NULL) {
        return SOLENOID_DRIVER_FAIL;
    }

    // // FILL VALVE
    // pca9574_set_mode_pin(solenoid_driver->pca9574, PCA9574_OUTPUT, SOLENOID_DRIVER_VALVE_GPIO_PIN_FILL);
    // pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, SOLENOID_DRIVER_VALVE_GPIO_PIN_FILL);
    // // FILL LED
    // pca9574_set_mode_pin(solenoid_driver->pca9574, PCA9574_OUTPUT, SOLENOID_DRIVER_LED_GPIO_PIN_FILL);
    // pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, SOLENOID_DRIVER_LED_GPIO_PIN_FILL);
    // // DEPR VALVE
    // pca9574_set_mode_pin(solenoid_driver->pca9574, PCA9574_OUTPUT, SOLENOID_DRIVER_VALVE_GPIO_PIN_DEPR);
    // pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, SOLENOID_DRIVER_VALVE_GPIO_PIN_DEPR);
    // // DEPR LED
    // pca9574_set_mode_pin(solenoid_driver->pca9574, PCA9574_OUTPUT, SOLENOID_DRIVER_LED_GPIO_PIN_DEPR);
    // pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, SOLENOID_DRIVER_LED_GPIO_PIN_DEPR);
    // // ADD VALVE
    // pca9574_set_mode_pin(solenoid_driver->pca9574, PCA9574_OUTPUT, SOLENOID_DRIVER_VALVE_GPIO_PIN_ADD);
    // pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, SOLENOID_DRIVER_VALVE_GPIO_PIN_ADD);
    // // ADD LED
    // pca9574_set_mode_pin(solenoid_driver->pca9574, PCA9574_OUTPUT, SOLENOID_DRIVER_LED_GPIO_PIN_ADD);
    // pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, SOLENOID_DRIVER_LED_GPIO_PIN_ADD);

    pca9574_set_mode(solenoid_driver->pca9574, PCA9574_OUTPUT);

    return SOLENOID_DRIVER_OK;
}

solenoid_driver_status_t solenoid_driver_valve_open(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve) {
    if (solenoid_driver == NULL) {
        return SOLENOID_DRIVER_FAIL;
    }

    if (solenoid_driver->valves[valve].state == SOLENOID_DRIVER_VALVE_STATE_OPEN) {
        return SOLENOID_DRIVER_OK;
    } else {
        pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_HIGH, solenoid_driver->valves[valve].gpio_pin);
        pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_HIGH, solenoid_driver->valves[valve].led_gpio_pin);
        solenoid_driver->valves[valve].state = SOLENOID_DRIVER_VALVE_STATE_OPEN;
    }

    return SOLENOID_DRIVER_OK;
}

solenoid_driver_status_t solenoid_driver_valve_close(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve) {
    if (solenoid_driver == NULL) {
        return SOLENOID_DRIVER_FAIL;
    }

    if (solenoid_driver->valves[valve].state == SOLENOID_DRIVER_VALVE_STATE_CLOSE) {
        return SOLENOID_DRIVER_OK;
    } else {
        pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, solenoid_driver->valves[valve].gpio_pin);
        pca9574_set_level_pin(solenoid_driver->pca9574, PCA9574_LOW, solenoid_driver->valves[valve].led_gpio_pin);
        solenoid_driver->valves[valve].state = SOLENOID_DRIVER_VALVE_STATE_CLOSE;
    }

    return SOLENOID_DRIVER_OK;
}

solenoid_driver_status_t solenoid_driver_valve_toggle(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve) {
    if (solenoid_driver == NULL) {
        return SOLENOID_DRIVER_FAIL;
    }

    if (solenoid_driver->valves[valve].state == SOLENOID_DRIVER_VALVE_STATE_OPEN) {
        solenoid_driver_valve_close(solenoid_driver, valve);
    } else {
        solenoid_driver_valve_open(solenoid_driver, valve);
    }

    return SOLENOID_DRIVER_OK;
}

solenoid_driver_status_t solenoid_driver_valve_get_state(solenoid_driver_struct_t *solenoid_driver, solenoid_driver_valve_t valve, solenoid_driver_valve_state_t *state) {
    if (solenoid_driver == NULL) {
        return SOLENOID_DRIVER_FAIL;
    }

    if (state == NULL) {
        return SOLENOID_DRIVER_FAIL;
    }

    *state = solenoid_driver->valves[valve].state;

    return SOLENOID_DRIVER_OK;
}