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

#include "can_commands.h"
#include "TANWA_data.h"

#include "esp_log.h"
#include "esp_timer.h"

#define TAG "CAN_TASK"

#define CAN_TASK_STACK_SIZE 4096
#define CAN_TASK_PRIORITY 8
#define CAN_TASK_CORE 1
#define CAN_TASK_DEFAULT_FREQ 100

static TaskHandle_t can_task_handle = NULL;
static SemaphoreHandle_t can_task_freq_mutex = NULL, can_task_rx_counter_mutex = NULL;
static volatile TickType_t can_task_freq = CAN_TASK_DEFAULT_FREQ;
static volatile uint32_t can_task_rx_counter = 0;
static int64_t rck_timer_us = 0;
static int64_t oxi_timer_us = 0;
static int64_t fac_timer_us = 0;
static int64_t flc_timer_us = 0;
static int64_t termo_timer_us = 0;

bool is_rx_counter_zero(void) {
    bool result = false;
    if (xSemaphoreTake(can_task_rx_counter_mutex, (TickType_t) 10) == pdTRUE) {
        result = (can_task_rx_counter == 0);
        xSemaphoreGive(can_task_rx_counter_mutex);
    }
    return result;
}

void run_can_task(void) {
    if (twai_start() != ESP_OK) {
      ESP_LOGE(TAG, "TWAI start error");
    } else {
        can_task_freq_mutex = xSemaphoreCreateMutex();
        can_task_rx_counter_mutex = xSemaphoreCreateMutex();
        int64_t timer_us = esp_timer_get_time();
        rck_timer_us = timer_us;
        oxi_timer_us = timer_us;
        fac_timer_us = timer_us;
        flc_timer_us = timer_us;
        termo_timer_us = timer_us;
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

static void change_can_task_period(uint32_t period_ms) {
    if (xSemaphoreTake(can_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        can_task_freq = (TickType_t) period_ms;
        xSemaphoreGive(can_task_freq_mutex);
        // ESP_LOGI(TAG, "CAN task period changed to: %d", period_ms);
    }
}

static void can_task_add_rx_counter(void) {
    if (xSemaphoreTake(can_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        can_task_rx_counter++;
        xSemaphoreGive(can_task_freq_mutex);
        // ESP_LOGI(TAG, "CAN RX counter++: %d", can_task_rx_counter);
    }
}

bool can_task_add_message(twai_message_t *message) {
    if (twai_transmit(message, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send the message");
        return false;
    }
    return true;
}

bool can_task_add_message_with_rx(twai_message_t *message) {
    if (twai_transmit(message, pdMS_TO_TICKS(100)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send the message");
        return false;
    }
    can_task_add_rx_counter();
    change_can_task_period(100U);
    return true;
}

static void can_task_sub_rx_counter(void) {
    if (xSemaphoreTake(can_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        can_task_rx_counter--;
        xSemaphoreGive(can_task_freq_mutex);
        // ESP_LOGI(TAG, "CAN RX counter--: %d", can_task_rx_counter);
    }
}

static void can_update_rck_timer(void) {
    rck_timer_us = esp_timer_get_time();
}

static void can_update_oxi_timer(void) {
    oxi_timer_us = esp_timer_get_time();
}

static void can_update_fac_timer(void) {
    fac_timer_us = esp_timer_get_time();
}

static void can_update_flc_timer(void) {
    flc_timer_us = esp_timer_get_time();
}

static void can_update_termo_timer(void) {
    termo_timer_us = esp_timer_get_time();
}

static void can_check_conection(void) {
    int64_t current_time = esp_timer_get_time();
    bool rck = false, oxi = false, fac = false, flc = false, termo = false;
    if (current_time - rck_timer_us < 5000000) {
        rck = true;
    }
    if (current_time - oxi_timer_us < 5000000) {
        oxi = true;
    }
    if (current_time - fac_timer_us < 5000000) {
        fac = true;
    }
    if (current_time - flc_timer_us < 5000000) {
        flc = true;
    }
    if (current_time - termo_timer_us < 5000000) {
        termo = true;
    }

    can_connected_slaves_t slaves = {
        .hx_rocket = rck,
        .hx_oxidizer = oxi,
        .fac = fac,
        .flc = flc,
        .termo = termo,
    };

    tanwa_data_update_can_connected_slaves(&slaves);
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

            can_check_conection();

            // Receive the CAN message from the queue
            twai_message_t rx_message;
            if (twai_receive(&rx_message, pdMS_TO_TICKS(100)) == ESP_OK) {
                // Parse the received message
                switch (rx_message.identifier) {
                    case CAN_HX_RCK_RX_STATUS: {
                        // ESP_LOGI(TAG, "Received HX RCK status");
                        can_task_sub_rx_counter();
                        parse_can_hx_rck_status(rx_message);
                        break;
                    }
                    case CAN_HX_RCK_RX_DATA: {
                        // ESP_LOGI(TAG, "Received HX RCK data");
                        can_task_sub_rx_counter();
                        parse_can_hx_rck_data(rx_message);
                        break;
                    }
                    case CAN_HX_RCK_RX_UPDATE: {
                        // ESP_LOGI(TAG, "Received HX RCK update");
                        can_update_rck_timer();
                        break;
                    }
                    case CAN_HX_OXI_RX_STATUS: {
                        // ESP_LOGI(TAG, "Received HX OXI status");
                        can_task_sub_rx_counter();
                        parse_can_hx_oxi_status(rx_message);
                        break;
                    }
                    case CAN_HX_OXI_RX_DATA: {
                        // ESP_LOGI(TAG, "Received HX OXI data");
                        can_task_sub_rx_counter();
                        parse_can_hx_oxi_data(rx_message);
                        break;
                    }
                    case CAN_HX_OXI_RX_UPDATE: {
                        // ESP_LOGI(TAG, "Received HX RCK update");
                        can_update_oxi_timer();
                        break;
                    }
                    case CAN_FAC_RX_STATUS: {
                        // ESP_LOGI(TAG, "Received FAC status");
                        can_task_sub_rx_counter();
                        parse_can_fac_status(rx_message);
                        break;
                    }
                    case CAN_FAC_RX_UPDATE: {
                        // ESP_LOGI(TAG, "Received HX RCK update");
                        can_update_fac_timer();
                        break;
                    }
                    case CAN_FLC_RX_STATUS: {
                        //ESP_LOGI(TAG, "Received FLC status");
                        can_task_sub_rx_counter();
                        parse_can_flc_status(rx_message);
                        break;
                    }
                    case CAN_FLC_RX_DATA: {
                        //ESP_LOGI(TAG, "Received FLC data");
                        can_task_sub_rx_counter();
                        parse_can_flc_data(rx_message);
                        break;
                    }
                    case CAN_FLC_RX_PRESSURE_DATA: {
                        //ESP_LOGI(TAG, "Received FLC pressure data");
                        can_task_sub_rx_counter();
                        parse_can_flc_pressure_data(rx_message);
                        break;
                    }
                    case CAN_FLC_RX_UPDATE: {
                        // ESP_LOGI(TAG, "Received HX RCK update");
                        can_update_flc_timer();
                        break;
                    }
                    case CAN_TERMO_RX_STATUS: {
                        //ESP_LOGI(TAG, "Received TERMO status");
                        can_task_sub_rx_counter();
                        parse_can_termo_status(rx_message);
                        break;
                    }
                    case CAN_TERMO_RX_DATA: {
                        //ESP_LOGI(TAG, "Received TERMO data");
                        can_task_sub_rx_counter();
                        parse_can_termo_data(rx_message);
                        break;
                    }
                    case CAN_TERMO_RX_UPDATE: {
                        // ESP_LOGI(TAG, "Received HX RCK update");
                        can_update_termo_timer();
                        break;
                    }
                    default: {
                        ESP_LOGW(TAG, "Unknown message ID: %d", rx_message.identifier);
                        can_task_sub_rx_counter();
                        break;
                    }
                }
            }

            // If counter is zero -> change the period back to the default
            if (is_rx_counter_zero()) {
                change_can_task_period(CAN_TASK_DEFAULT_FREQ);
            }
        }
    }
}