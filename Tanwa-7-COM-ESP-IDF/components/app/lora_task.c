///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "lora_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "TANWA_config.h"

#define TAG "LORA_TASK"

void run_lora_task(void) {
    xTaskCreatePinnedToCore(lora_task, "lora_task", 4096, NULL, 5, NULL, 1);
}

void lora_task(void* pvParameters) {
    ESP_LOGI(TAG, "### LoRa task started ###");
    while (1) {
        // ESP_LOGI(TAG, "sending packet: %s", lora_packet);
        // lora_send_packet(&devices_config->lora, (uint8_t*)lora_packet,
        //                  strlen(lora_packet));
        // ESP_LOGI(TAG, "packet sent...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}