// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "memory_task.h"

#define MEMORY_TASK_TAG "MEMORY_TASK"

void memory_task(void* pcParameters) {
  ROSALIA_devices_t* devices = (ROSALIA_devices_t*)pcParameters;
  ESP_LOGI(MEMORY_TASK_TAG, "Memory task started");

  SD_unmount(devices->memory.sd_card);
  memory_usb_msc_activate(&devices->memory, USB_MSC_INIT_STORAGE_TYPE_SDMMC);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    bool ret =
        usb_msc_check_storage_use_by_host(devices->memory.usb_msc_config);
    ESP_LOGI(
        MEMORY_TASK_TAG,
        "Memory task running, sd card should be on USB MSC: return value is %d",
        ret);
  }
}
