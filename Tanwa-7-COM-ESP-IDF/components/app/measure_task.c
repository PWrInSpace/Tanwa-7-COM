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

    float temp[2], pressure;

    // Initialise the xLastWakeTime variable with the current time.
    last_wake_time = xTaskGetTickCount();

    while (1) {
        // Wait for the next cycle.
        if (xSemaphoreTake(measure_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
            local_freq = measure_task_freq;
            xSemaphoreGive(measure_task_freq_mutex);
        
            vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(local_freq));

            // Measure temperature
            for (int i = 0; i < 2; ++i) {
                tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[i]), &temp[i]);
            }
            ESP_LOGI(TAG, "Temperature sensors 1: %.2f, 2: %.2f", temp[0], temp[1]); 

            // Measure pressure
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &pressure);
            ESP_LOGI(TAG, "Pressure: %.2f", pressure);

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