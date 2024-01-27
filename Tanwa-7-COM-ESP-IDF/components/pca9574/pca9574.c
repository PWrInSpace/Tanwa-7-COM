///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 14.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#include "pca9574.h"
#include "esp_log.h"

#define TAG "PCA9574"

///===-----------------------------------------------------------------------------------------===//

bool pca9574_init(pca9574_struct_t *pca9574) {
    assert(pca9574->_i2c_read != NULL);
    assert(pca9574->_i2c_write != NULL);
    if (pca9574->_i2c_read == NULL || pca9574->_i2c_write == NULL) {
        return false;
    }

    uint8_t input_port = 0;
    if (pca9574->_i2c_read(pca9574->i2c_address, PCA9574_INPUT_PORT_REG, &input_port, 1) == false) {
        return false;
    }

    return true;
}

bool pca9574_set_mode(pca9574_struct_t *pca9574, pca9574_pin_mode_t mode) {
    uint8_t reg = 0x00;

    if (mode == PCA9574_INPUT) {
        reg = 0xFF;
    }

    if (pca9574->_i2c_write(pca9574->i2c_address, PCA9574_CONFIG_REG, &reg, 1) == false) {
        return false;
    }

    // Disable high impedance (perhaps) on pin, idk why set to output does not set pins to low, but also pins are not at hight level
    if (mode == PCA9574_OUTPUT) {
        reg = 0x00;
        if (pca9574->_i2c_write(pca9574->i2c_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
            return false;
        }
    }

    return true;
}

bool pca9574_set_mode_pin(pca9574_struct_t *pca9574, pca9574_pin_mode_t mode, uint8_t pin) {
    uint8_t reg = 0x00;
    if (pca9574->_i2c_read(pca9574->i2c_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    if (mode == PCA9574_OUTPUT) {
        reg &= ~(mode << pin);
    } else {
        reg |= (mode << pin);
    }

    if (pca9574->_i2c_write(pca9574->i2c_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool pca9574_set_level(pca9574_struct_t *pca9574, pca9574_pin_level_t level) {
    uint8_t reg = 0x00;

    if (level == PCA9574_HIGH) {
        reg = 0xFF;
    }

    if (pca9574->_i2c_write(pca9574->i2c_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}

bool pca9574_set_level_pin(pca9574_struct_t *pca9574, pca9574_pin_level_t level, uint8_t pin) {
    uint8_t reg = 0x00;
    if (pca9574->_i2c_read(pca9574->i2c_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    if (level == PCA9574_LOW) {
        reg &= ~(1 << pin);
    } else {
        reg |= (1 << pin);
    }

    if (pca9574->_i2c_write(pca9574->i2c_address, PCA9574_OUTPUT_PORT_REG, &reg, 1) == false) {
        return false;
    }

    return true;
}