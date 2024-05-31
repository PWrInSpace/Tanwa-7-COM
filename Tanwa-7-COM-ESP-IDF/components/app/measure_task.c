///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "measure_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "TANWA_config.h"
#include "TANWA_data.h"
#include "mcu_adc_config.h"

#define TAG "MEASURE_TASK"

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

static volatile TickType_t measure_task_freq = 10000;
SemaphoreHandle_t measure_task_freq_mutex;

void run_measure_task(void) {
    measure_task_freq_mutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(measure_task, "measure_task", 4096, NULL, 5, NULL, 1);
}

void change_measure_task_period(uint32_t period) {
    if (xSemaphoreTake(measure_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        measure_task_freq = (TickType_t) period;
        xSemaphoreGive(measure_task_freq_mutex);
    }
}

void measure_task(void* pvParameters) {
    ESP_LOGI(TAG, "### Measurement task started ###");

    TickType_t last_wake_time;
    TickType_t local_freq;

    float vbat, temp[2], pressure[4];

    // Initialise the xLastWakeTime variable with the current time.
    last_wake_time = xTaskGetTickCount();

    while (1) {
        // Wait for the next cycle.
        if (xSemaphoreTake(measure_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
            local_freq = measure_task_freq;
            xSemaphoreGive(measure_task_freq_mutex);
        
            vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(local_freq));

            com_data_t com_data;

            // Measure battery voltage
            _mcu_adc_read_voltage(VBAT_CHANNEL_INDEX, &vbat);
            vbat = vbat * 6.26335877863f; // (10k + 50k) / 10k (voltage divider)
            // ESP_LOGI(TAG, "Battery voltage: %.2f", vbat);
            com_data.vbat = vbat;

            // Measure temperature
            for (int i = 0; i < 2; ++i) {
                tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[i]), &temp[i]);
            }
            // ESP_LOGI(TAG, "Temperature sensors 1: %.2f, 2: %.2f", temp[0], temp[1]);
            com_data.temperature_1 = temp[0];
            com_data.temperature_2 = temp[1];

            // Measure pressure
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &pressure[0]);
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure[1]);
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_3, &pressure[2]);
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_4, &pressure[3]);
            // ESP_LOGI(TAG, "Pressure: %.2f", pressure);
            com_data.pressure_1 = pressure[0];
            com_data.pressure_2 = pressure[1];
            com_data.pressure_3 = pressure[2];
            com_data.pressure_4 = pressure[3];

            // Update COM data
            tanwa_data_update_com_data(&com_data);

            // Oxidizer weight measurement
            // CAN bus communication
            // ToDo

            // Rocket weight measurement
            // CAN bus communication
            // ToDo

            // Fetch data from CAN bus
            // ToDo
        }
    }
    vTaskDelete(NULL);
}