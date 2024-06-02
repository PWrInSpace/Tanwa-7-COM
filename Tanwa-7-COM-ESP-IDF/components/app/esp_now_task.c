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
#include "freertos/queue.h"

#include "esp_log.h"

#include "now.h"
#include "now_commands.h"
#include "state_machine_config.h"

#define TAG "ESP_NOW_TASK"

#define NOW_TASK_STACK_SIZE 4096
#define NOW_TASK_PRIORITY 5
#define NOW_TASK_CORE 0
#define NOW_TASK_DEFAULT_FREQ 100

static TaskHandle_t now_task_handle = NULL;
static SemaphoreHandle_t now_task_freq_mutex = NULL;
static volatile TickType_t now_task_freq = NOW_TASK_DEFAULT_FREQ;

extern QueueHandle_t obc_now_rx_queue;

void run_esp_now_task(void) {
    now_task_freq_mutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(esp_now_task, "esp_now_task", NOW_TASK_STACK_SIZE, NULL, 
                            NOW_TASK_PRIORITY, &now_task_handle, NOW_TASK_CORE);
}

void stop_esp_now_task(void) {
    vTaskDelete(now_task_handle);
}

void change_esp_now_task_period(uint32_t period_ms) {
    if (xSemaphoreTake(now_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        now_task_freq = (TickType_t) period_ms;
        xSemaphoreGive(now_task_freq_mutex);
    }
}

void esp_now_task(void* pvParameters) {
    ESP_LOGI(TAG, "### ESP-NOW task started ###");

    TickType_t last_wake_time;
    TickType_t local_freq;

    // Initialise the xLastWakeTime variable with the current time.
    last_wake_time = xTaskGetTickCount();

    DataFromObc obc_command;

    while (1) {
        // Wait for the next cycle.
        if (xSemaphoreTake(now_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
            local_freq = now_task_freq;
            xSemaphoreGive(now_task_freq_mutex);

            vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(local_freq));

            if (xQueueReceive(obc_now_rx_queue, (void*)&obc_command, (TickType_t) 10) == pdTRUE) {
                switch (obc_command.commandNum) {
                    case NOW_STATE_CHANGE: {
                        state_t state = (state_t) obc_command.commandArg;
                        char state_text[20];
                        get_state_text(state, state_text);
                        ESP_LOGI(TAG, "ESP-NOW | State change | %s", state_text);
                        break;
                    }
                    case NOW_ABORT: {
                        ESP_LOGI(TAG, "ESP-NOW | Abort");
                        break;
                    }
                    case NOW_HOLD_IN: {
                        ESP_LOGI(TAG, "ESP-NOW | Hold in");
                        break;
                    }
                    case NOW_HOLD_OUT: {
                        ESP_LOGI(TAG, "ESP-NOW | Hold out");
                        if (state_machine_get_current_state() == HOLD) {
                            ESP_LOGW(TAG, "SM | Not in HOLD state");
                            break;
                        } else {
                            state_machine_status_t sm_status = state_machine_change_to_previous_state(true);
                            if (sm_status != STATE_MACHINE_OK) {
                                ESP_LOGE(TAG, "SM | State change error | %d", sm_status);
                            }
                        }
                        break;
                    }
                    default: {
                        ESP_LOGI(TAG, "ESP-NOW command: %d", obc_command.commandNum);
                        ESP_LOGW(TAG, "ESP-NOW | Unknown command");
                        break;
                    }
                }
            }
        }
    }
}