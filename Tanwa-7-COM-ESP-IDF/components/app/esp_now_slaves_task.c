///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "esp_now_slaves_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "now.h"
#include "cmd_commands.h"
#include "state_machine_config.h"
#include "TANWA_data.h"

#define TAG "ESP_NOW_SLAVES_TASK"

#define NOW_TASK_STACK_SIZE 4096
#define NOW_TASK_PRIORITY 1
#define NOW_TASK_CORE 0
#define NOW_TASK_DEFAULT_FREQ 100

static TaskHandle_t now_task_handle = NULL;
static TickType_t now_task_freq = NOW_TASK_DEFAULT_FREQ;

extern QueueHandle_t main_valve_1_now_rx_queue;
extern QueueHandle_t main_valve_2_now_rx_queue;

void run_esp_now_slaves_task(void) {
    xTaskCreatePinnedToCore(esp_now_slaves_task, "esp_now_task", NOW_TASK_STACK_SIZE, NULL, 
                            NOW_TASK_PRIORITY, &now_task_handle, NOW_TASK_CORE);
}

void stop_esp_now_slaves_task(void) {
    vTaskDelete(now_task_handle);
}

void esp_now_slaves_task(void* pvParameters) {
    ESP_LOGI(TAG, "### ESP-NOW task started ###");

    TickType_t last_wake_time;
    TickType_t local_freq;

    last_wake_time = xTaskGetTickCount();

    while (1) {

        vTaskDelayUntil(&last_wake_time, now_task_freq);
        // check if there is any data in the queue
        now_main_valve_pressure_data_t data_from_main_valve_1;
        if (xQueueReceive(main_valve_1_now_rx_queue, &data_from_main_valve_1, 0) == pdTRUE) {
            ESP_LOGI(TAG, "Received data from MAIN_VALVE_1: \n PRESSURE_1: %d, PRESSURE_2: ", data_from_main_valve_1.pressure_1, data_from_main_valve_1.pressure_2);
            tanwa_data_update_now_main_valve_pressure_data(&data_from_main_valve_1);
        }

        now_main_valve_temperature_data_t data_from_main_valve_2;
        if (xQueueReceive(main_valve_2_now_rx_queue, &data_from_main_valve_2, 0) == pdTRUE) {
            ESP_LOGI(TAG, "Received data from MAIN_VALVE_2: TEMPERATURE 1: %d, TEMPERATURE 2: %d", data_from_main_valve_2.temperature_1, data_from_main_valve_2.temperature_2);
            tanwa_data_update_now_main_valve_temperature_data(&data_from_main_valve_2);
        }
    }
}