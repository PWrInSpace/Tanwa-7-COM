///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "esp_now_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#define TAG "ESP_NOW_TASK"

#define ESP_NOW_TASK_STACK_SIZE 4096
#define ESP_NOW_TASK_PRIORITY 5
#define ESP_NOW_TASK_CORE 1

static TaskHandle_t esp_now_task_handle = NULL;

void run_esp_now_task(void) {
    xTaskCreatePinnedToCore(esp_now_task, "esp_now_task", ESP_NOW_TASK_STACK_SIZE, NULL, 
                            ESP_NOW_TASK_PRIORITY, &esp_now_task_handle, ESP_NOW_TASK_CORE);
}

void stop_esp_now_task(void) {
    vTaskDelete(esp_now_task_handle);
}

void esp_now_task(void* pvParameters) {
    ESP_LOGI(TAG, "### ESP-NOW task started ###");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}