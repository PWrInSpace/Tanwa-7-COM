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
#include "cmd_commands.h"
#include "state_machine_config.h"

#define TAG "ESP_NOW_TASK"

#define NOW_TASK_STACK_SIZE 4096
#define NOW_TASK_PRIORITY 1
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
                    case CMD_STATE_CHANGE: {
                        state_t state = (state_t) obc_command.commandArg;
                        char state_text[20];
                        get_state_text(state, state_text);
                        ESP_LOGI(TAG, "ESP-NOW | State change | %s", state_text);
                        tanwa_state_change((uint8_t) state);
                        break;
                    }
                    case CMD_ABORT: {
                        ESP_LOGI(TAG, "ESP-NOW | Abort");
                        tanwa_abort();
                        break;
                    }
                    case CMD_HOLD_IN: {
                        ESP_LOGI(TAG, "ESP-NOW | Hold in");
                        tanwa_hold_in();
                        break;
                    }
                    case CMD_HOLD_OUT: {
                        ESP_LOGI(TAG, "ESP-NOW | Hold out");
                        tanwa_hold_out();
                        break;
                    }
                    case CMD_SOFT_ARM: {
                        ESP_LOGI(TAG, "ESP-NOW | Soft arm");
                        tanwa_soft_arm();
                        break;
                    }
                    case CMD_SOFT_DISARM: {
                        ESP_LOGI(TAG, "ESP-NOW | Soft disarm");
                        tanwa_soft_disarm();
                        break;
                    }
                    case CMD_FIRE: {
                        ESP_LOGI(TAG, "ESP-NOW | Fire");
                        tanwa_fire();
                        break;
                    }
                    case CMD_FILL: {
                        if (obc_command.commandArg == CMD_VALVE_OPEN) {
                            ESP_LOGI(TAG, "ESP-NOW | Fill open ");
                        } else {
                            ESP_LOGI(TAG, "ESP-NOW | Fill close ");
                        }
                        tanwa_fill((uint8_t) obc_command.commandArg);
                        break;
                    }
                    case CMD_FILL_TIME: {
                        ESP_LOGI(TAG, "ESP-NOW | Fill time open | %d", obc_command.commandArg);
                        tanwa_fill_time((uint8_t) obc_command.commandArg);
                        break;
                    }
                    case CMD_DEPR: {
                        if (obc_command.commandArg == CMD_VALVE_OPEN) {
                            ESP_LOGI(TAG, "ESP-NOW | Depr open ");
                        } else {
                            ESP_LOGI(TAG, "ESP-NOW | Depr close ");
                        }
                        tanwa_depr((uint8_t) obc_command.commandArg);
                        break;
                    }
                    case CMD_QD: {
                        switch ((uint8_t) obc_command.commandArg) {
                            case CMD_QD_PUSH: {
                                ESP_LOGI(TAG, "ESP-NOW | QD push");
                                break;
                            }
                            case CMD_QD_PULL: {
                                ESP_LOGI(TAG, "ESP-NOW | QD pull");
                                break;
                            }
                            case CMD_QD_STOP: {
                                ESP_LOGI(TAG, "ESP-NOW | QD stop");
                                break;
                            }
                            default: {
                                ESP_LOGW(TAG, "ESP-NOW | Unknown QD command");
                                break;
                            }
                        }
                        tanwa_qd_1((uint8_t) obc_command.commandArg);
                        break;
                    }
                    case CMD_SOFT_RESTART_RCK: {
                        ESP_LOGI(TAG, "ESP-NOW | Soft restart RCK");
                        tanwa_soft_restart_rck();
                        break;
                    }
                    case CMD_SOFT_RESTART_OXI: {
                        ESP_LOGI(TAG, "ESP-NOW | Soft restart OXI");
                        tanwa_soft_restart_oxi();
                        break;
                    }
                    case CMD_SOFT_RESTART_ESP: {
                        ESP_LOGI(TAG, "ESP-NOW | Soft restart ESP");
                        tanwa_soft_restart_esp();
                        break;
                    }
                    case CMD_CALIBRATE_RCK: {
                        ESP_LOGI(TAG, "ESP-NOW | Calibrate RCK | Weight: %d", obc_command.commandArg);
                        tanwa_calibrate_rck((float) obc_command.commandArg);
                        break;
                    }
                    case CMD_TARE_RCK: {
                        ESP_LOGI(TAG, "ESP-NOW | Tare RCK");
                        tanwa_tare_rck();
                        break;
                    }
                    case CMD_SET_CAL_FACTOR_RCK: {
                        ESP_LOGI(TAG, "ESP-NOW | Set cal factor RCK | %d", obc_command.commandArg);
                        tanwa_set_cal_factor_rck((float) obc_command.commandArg);
                        break;
                    }
                    case CMD_SET_OFFSET_RCK: {
                        ESP_LOGI(TAG, "ESP-NOW | Set offset RCK | %d", obc_command.commandArg);
                        tanwa_set_offset_rck((float) obc_command.commandArg);
                        break;
                    }
                    case CMD_CALIBRATE_OXI: {
                        ESP_LOGI(TAG, "ESP-NOW | Calibrate OXI | Weight: %d", obc_command.commandArg);
                        tanwa_calibrate_oxi((float) obc_command.commandArg);
                        break;
                    }
                    case CMD_TARE_OXI: {
                        ESP_LOGI(TAG, "ESP-NOW | Tare OXI");
                        tanwa_tare_oxi();
                        break;
                    } 
                    case CMD_SET_CAL_FACTOR_OXI: {
                        ESP_LOGI(TAG, "ESP-NOW | Set cal factor OXI | %d", obc_command.commandArg);
                        tanwa_set_cal_factor_oxi((float) obc_command.commandArg);
                        break;
                    }
                    case CMD_SET_OFFSET_OXI: {
                        ESP_LOGI(TAG, "ESP-NOW | Set offset OXI | %d", obc_command.commandArg);
                        tanwa_set_offset_oxi((float) obc_command.commandArg);
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