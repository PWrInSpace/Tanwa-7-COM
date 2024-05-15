///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "esp_now_task.h"

void run_esp_now_task(void) {
    xTaskCreatePinnedToCore(esp_now_task, "esp_now_task", 4096, NULL, 5, NULL, 1);
}

void esp_now_task(void* pvParameters);