///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 27.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of a simple ADC driver for voltage measurement of the battery.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_VOLTAGE_MEASURE_DRIVER_H_
#define PWRINSPACE_VOLTAGE_MEASURE_DRIVER_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    VOLTAGE_MEASURE_OK = 0,
    VOLTAGE_MEASURE_FAIL,
    VOLTAGE_MEASURE_ADC_ERR,
    VOLTAGE_MEASURE_NULL_ARG,
} voltage_measure_status_t;

typedef bool (*voltage_measure_ADC_analog_read_raw)(uint8_t _adc_channel, uint16_t* _value);
typedef void (*voltage_measure_delay)(uint32_t _ms);

typedef struct {
    voltage_measure_ADC_analog_read_raw _adc_analog_read_raw;
    voltage_measure_delay _delay;
    uint8_t adc_channel;
} voltage_measure_struct_t;

voltage_measure_status_t voltage_measure_read_raw(voltage_measure_struct_t* voltage_measure, uint16_t* value);

voltage_measure_status_t voltage_measure_read_voltage(voltage_measure_struct_t* voltage_measure, float* value);

#endif /* PWRINSPACE_VOLTAGE_MEASURE_DRIVER_H_ */