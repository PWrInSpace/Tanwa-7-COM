///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 13.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of TMP1075 class which is a driver for TMP1075 temperature sensor.
/// The alert functionalisty is not implemented.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TMP1075_H_
#define PWRINSPACE_TMP1075_H_

#include "sensor_status.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* TMP1075 REGISTERS */
#define TMP1075_REG_TEMP 0x00
#define TMP1075_REG_CONFIG 0x01

typedef enum {
    Fault_1 = 0b00,
    Fault_2 = 0b01,
    Fault_3 = 0b10,
    Fault_4 = 0b11,
} tmp1075_consecutive_faults_t;

typedef enum {
    ConversionTime27_5ms = 0b00,
    ConversionTime55ms = 0b01,
    ConversionTime110ms = 0b10,
    ConversionTime220ms = 0b11,
} tmp1075_conversion_time_t;

typedef enum {
    OS = 7,
    R  = 5,
    F  = 3,
    POL = 2,
    TM = 1,
    SD = 0,
} tmp1075_offsets_t;

// I2C access functions
typedef bool (*tmp1075_I2C_write)(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
typedef bool (*tmp1075_I2C_read)(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

typedef struct {
    tmp1075_I2C_write _i2c_write;
    tmp1075_I2C_read _i2c_read;
    uint8_t i2c_address;
    uint8_t config_register;
} tmp1075_struct_t;

sensor_status_t tmp1075_init(tmp1075_struct_t *tmp1075);

sensor_status_t tmp1075_get_temp_raw(tmp1075_struct_t *tmp1075, int16_t *raw);

sensor_status_t tmp1075_get_temp_celsius(tmp1075_struct_t *tmp1075, float *temp);

sensor_status_t tmp1075_start_conv(tmp1075_struct_t *tmp1075);

sensor_status_t tmp1075_get_conv_mode(tmp1075_struct_t *tmp1075, bool *is_single_shot);

sensor_status_t tmp1075_set_conv_mode(tmp1075_struct_t *tmp1075, const bool is_single_shot);

sensor_status_t tmp1075_get_conv_time(tmp1075_struct_t *tmp1075, tmp1075_conversion_time_t *conv_time);

sensor_status_t tmp1075_set_conv_time(tmp1075_struct_t *tmp1075, const tmp1075_conversion_time_t conv_time);

#endif /* PWRINSPACE_TMP1075_H_ */
