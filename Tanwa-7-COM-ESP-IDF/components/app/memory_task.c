// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "memory_task.h"

#define MEMORY_TASK_TAG "MEMORY_TASK"

void memory_task(void* pcParameters) {
  TANWA_devices_t* devices = (TANWA_devices_t*)pcParameters;
  ESP_LOGI(MEMORY_TASK_TAG, "Memory task started");

  SD_unmount(devices->memory.sd_card);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
