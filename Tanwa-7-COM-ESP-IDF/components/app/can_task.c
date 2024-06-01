///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 31.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "can_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "mcu_twai_config.h"
#include "can_commands.h"

#define TAG "CAN_TASK"

#define CAN_TASK_STACK_SIZE 4096
#define CAN_TASK_PRIORITY 5
#define CAN_TASK_CORE 1

static TaskHandle_t can_task_handle = NULL;
static SemaphoreHandle_t can_task_freq_mutex = NULL;
static volatile TickType_t can_task_freq = 10000;
static volatile uint32_t can_task_counter = 0;

void run_can_task(void) {
    if (twai_start() != ESP_OK) {
      ESP_LOGE(TAG, "TWAI start error");
    } else {
    xTaskCreatePinnedToCore(can_task, "can_task", CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIORITY, 
                            &can_task_handle, CAN_TASK_CORE);
    }
}

void stop_can_task(void) {
    vTaskDelete(can_task_handle);
    if (twai_stop() != ESP_OK) {
      ESP_LOGE(TAG, "TWAI stop error");
    }
}

void change_can_task_period(uint32_t period_ms) {
    if (xSemaphoreTake(can_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        can_task_freq = (TickType_t) period_ms;
        xSemaphoreGive(can_task_freq_mutex);
    }
}

void can_task_add_counter(void) {
    can_task_counter++;
}

void can_task(void* pvParameters) {
    ESP_LOGI(TAG, "### CAN task started ###");

    TickType_t last_wake_time;
    TickType_t local_freq;

    // Initialise the xLastWakeTime variable with the current time.
    last_wake_time = xTaskGetTickCount();

    while (1) {
        // Wait for the next cycle.
        if (xSemaphoreTake(can_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
            local_freq = can_task_freq;
            xSemaphoreGive(can_task_freq_mutex);

            vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(local_freq));

            // If there are more than 1 messages in the queue, shorten the delay
            // twai_status_info_t status_info;
            // twai_get_status_info(&status_info);
            // if (status_info.msgs_to_rx > 1) {
            //     change_can_task_period(100);
            // }

            // Receive the CAN message from the queue
            twai_message_t rx_message;
            if (twai_receive(&rx_message, pdMS_TO_TICKS(100)) == ESP_OK) {
                ESP_LOGI(TAG, "Received message with ID: %d", rx_message.identifier);

                // Parse the received message
                switch (rx_message.identifier) {
                    default: {
                        ESP_LOGW(TAG, "Unknown message ID: %d", rx_message.identifier);
                        break;
                    }
                }
            }
        }
    }
}