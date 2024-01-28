///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#include "voltage_measure_driver.h"
#include "esp_log.h"

#define TAG "VOLTAGE_MEASURE"

voltage_measure_status_t voltage_measure_read_raw(voltage_measure_struct_t* voltage_measure, uint16_t* value) {
    if (voltage_measure == NULL || value == NULL) {
        ESP_LOGE(TAG, "Invalid argument - NULL check failed");
        return VOLTAGE_MEASURE_NULL_ARG;
    }
    if (!voltage_measure->_adc_analog_read_raw(voltage_measure->adc_channel, value)) {
        ESP_LOGE(TAG, "Failed to read analog value from ADC for voltage measurement");
        return VOLTAGE_MEASURE_ADC_ERR;
    }
    return VOLTAGE_MEASURE_OK;
}

voltage_measure_status_t voltage_measure_read_voltage(voltage_measure_struct_t* voltage_measure, float* value) {}