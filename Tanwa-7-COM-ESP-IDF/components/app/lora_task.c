///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "lora_task.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "TANWA_config.h"

#include "esp_log.h"

#define TAG "LORA_TASK"

#define LORA_TASK_STACK_SIZE 4096
#define LORA_TASK_PRIORITY 2
#define LORA_TASK_CORE 0
#define LORA_TASK_DEFAULT_FREQ 100

static TaskHandle_t lora_task_handle = NULL;
static SemaphoreHandle_t lora_task_freq_mutex = NULL;
static volatile TickType_t lora_task_freq = LORA_TASK_DEFAULT_FREQ;

void run_lora_task(void) {
    xTaskCreatePinnedToCore(lora_task, "lora_task", LORA_TASK_STACK_SIZE, NULL, 
                            LORA_TASK_PRIORITY, &lora_task_handle, LORA_TASK_CORE);
}

void stop_lora_task(void) {
    vTaskDelete(lora_task_handle);
}

void change_lora_task_period(uint32_t period_ms) {
    if (xSemaphoreTake(lora_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        lora_task_freq = (TickType_t) period_ms;
        xSemaphoreGive(lora_task_freq_mutex);
    }
}

void lora_task(void* pvParameters) {
    ESP_LOGI(TAG, "### LoRa task started ###");

    TickType_t last_wake_time;
    TickType_t local_freq;

    // Initialise the xLastWakeTime variable with the current time.
    last_wake_time = xTaskGetTickCount();

    while (1) {
        // Wait for the next cycle.
        if (xSemaphoreTake(lora_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
            local_freq = lora_task_freq;
            xSemaphoreGive(lora_task_freq_mutex);

            vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(local_freq));

            // ESP_LOGI(TAG, "sending packet: %s", lora_packet);
            // lora_send_packet(&devices_config->lora, (uint8_t*)lora_packet,
            //                  strlen(lora_packet));
            // ESP_LOGI(TAG, "packet sent...");
        }
    }
    vTaskDelete(NULL);
}