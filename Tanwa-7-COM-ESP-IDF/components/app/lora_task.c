// Copyright 2022 PWrInSpace, Kuba
#include "lora_task.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "TANWA_config.h"
#include "mcu_gpio_config.h"
#include "mcu_spi_config.h"
#include "mcu_misc_config.h"

#define TAG "LORA_TASK"

#define LORA_TASK_STACK_SIZE 8192
#define LORA_TASK_PRIORITY 2
#define LORA_TASK_CORE 0

static struct {
    lora_struct_t lora;
    lora_task_process_rx_packet process_packet_fnc;
    TaskHandle_t task;
} gb;

void IRAM_ATTR lora_task_irq_notify(void *arg) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    vTaskNotifyGiveFromISR(gb.task, &higher_priority_task_woken);
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static size_t on_lora_receive(uint8_t *rx_buffer, size_t buffer_len) {
    size_t len = 0;
    if (lora_received(&gb.lora) == LORA_OK) {
        len = lora_receive_packet(&gb.lora, rx_buffer, buffer_len);
        rx_buffer[len] = '\0';
        ESP_LOGD(TAG, "Received %s, len %d", rx_buffer, len);
        lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
        lora_set_receive_mode(&gb.lora);
    }
    return len;
}

static void lora_task(void *arg) {
    ESP_LOGI(TAG, "LoRa Task started");

    uint8_t rx_buffer[256];
    size_t rx_packet_size = 0;

    while (1) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "IRQ received");
            if (lora_received(&gb.lora) == LORA_OK) {
                rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));
                if (rx_packet_size > 0 && gb.process_packet_fnc != NULL) {
                    ESP_LOGI(TAG, "Processing packet");
                    gb.process_packet_fnc(rx_buffer, rx_packet_size);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool lora_task_init(lora_api_config_t *cfg) {
    assert(cfg != NULL);
    if (cfg == NULL) {
        return false;
    }

    if (cfg->process_rx_packet_fnc == NULL) {
        return false;
    }

    gb.process_packet_fnc = cfg->process_rx_packet_fnc;
    memcpy(&gb.lora, cfg->lora, sizeof(lora_struct_t));

    lora_init(&gb.lora);
    lora_set_frequency(&gb.lora, cfg->frequency_khz * 1e3);
    lora_set_bandwidth(&gb.lora, LORA_TASK_BANDWIDTH);
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    lora_set_receive_mode(&gb.lora);

    if (LORA_TASK_CRC_ENABLE) {
        lora_enable_crc(&gb.lora);
    } else {
        lora_disable_crc(&gb.lora);
    }

    ESP_LOGI(TAG, "Reading LoRa registers");
    int16_t read_val_one = lora_read_reg(&gb.lora, 0x0d);
    int16_t read_val_two = lora_read_reg(&gb.lora, 0x0c);
    ESP_LOGI(TAG, "LORA_READ: %04x, %04x", read_val_one, read_val_two);

    xTaskCreatePinnedToCore(lora_task, "LoRa task", LORA_TASK_STACK_SIZE, NULL, LORA_TASK_PRIORITY,
                            &gb.task, LORA_TASK_CORE);

    if (gb.task == NULL) {
        return false;
    }
    return true;
}

bool lora_change_frequency(uint32_t frequency_khz) {
    if (frequency_khz < 4e5 || frequency_khz > 1e6) {
        return false;
    }
    if (lora_set_frequency(&gb.lora, frequency_khz * 1000) != LORA_OK) {
        return false;
    }
    return true;
}