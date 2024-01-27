///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 27.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of PCA9574 I2C expander driver. The implementation is based on
/// the implementation of the PCA9574 driver made by Jakub Siuda. 
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_PCA9574_H_
#define PWRINSPACE_PCA9574_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PCA9574_INPUT_PORT_REG 0x00
#define PCA9574_POLARITY_INVERSION_REG 0x01
#define PCA9574_BUS_ENABLE_REG 0x02
#define PCA9574_PULL_SELECTOR_REG 0x03
#define PCA9574_CONFIG_REG 0x04
#define PCA9574_OUTPUT_PORT_REG 0x05
#define PCA9574_INTR_MASK_REG 0x06
#define PCA9574_INTR_STATUS_REG 0x07

typedef enum {
    PCA9574_OUTPUT = 0,
    PCA9574_INPUT,
} pca9574_pin_mode_t;

typedef enum {
    PCA9574_LOW = 0,
    PCA9574_HIGH,
} pca9574_pin_level_t;

typedef bool (*pca9574_i2c_read)(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
typedef bool (*pca9574_i2c_write)(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

typedef struct {
    pca9574_i2c_read _i2c_read;
    pca9574_i2c_write _i2c_write;
    uint8_t i2c_address;
} pca9574_struct_t;

/**
 * @brief Initalize PCA9574
 * 
 * @param pca9574 pointer to expander struct
 * @return true :D
 * @return false :C
 */
bool pca9574_init(pca9574_struct_t *pca);

/**
 * @brief Set all pin mode
 * 
 * @param pca9574 pointer to expander struct
 * @param mode gpio pins mode
 * @return true :D
 * @return false :C
 */
bool pca9574_set_mode(pca9574_struct_t *pca9574, pca9574_pin_mode_t mode);

/**
 * @brief Set pin mode
 * 
 * @param pca9574 pointer to expander struct
 * @param mode pin mode
 * @param pin pin
 * @return true :D
 * @return false :C
 */
bool pca9574_set_mode_pin(pca9574_struct_t *pca9574, pca9574_pin_mode_t mode, uint8_t pin);

/**
 * @brief Set all pin levels
 * 
 * @param pca9574 pointer to expander struct
 * @param level pins level
 * @return true :D
 * @return false :C
 */
bool pca9574_set_level(pca9574_struct_t *pca9574, pca9574_pin_level_t level);

/**
 * @brief Set pin level
 * 
 * @param pca9574 pointer to expander struct
 * @param level level
 * @param pin pin
 * @return true :D
 * @return false :C
 */
bool pca9574_set_level_pin(pca9574_struct_t *pca9574, pca9574_pin_level_t level, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* PWRINSPACE_PCA9574_H_ */