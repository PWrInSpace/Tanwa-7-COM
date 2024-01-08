// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "lora_task.h"

#define TAG "LORA_TASK"

void lora_task(void* pvParameters) {
  ROSALIA_devices_t* devices_config = (ROSALIA_devices_t*)pvParameters;
  const char* lora_packet = "TEST;";
  ESP_LOGI(TAG, "LORA_TASK");
  ESP_LOGI(TAG, "Reading LoRa registers");
  int16_t read_val_one = lora_read_reg(&devices_config->lora, 0x0d);
  int16_t read_val_two = lora_read_reg(&devices_config->lora, 0x0c);
  ESP_LOGI(TAG, "LORA_READ: %04x, %04x", read_val_one, read_val_two);
  lora_reset(&devices_config->lora);
  ESP_LOGI(TAG, "LoRa Reset");
  for (;;) {
    ESP_LOGI(TAG, "sending packet: %s", lora_packet);
    lora_send_packet(&devices_config->lora, (uint8_t*)lora_packet,
                     strlen(lora_packet));
    ESP_LOGI(TAG, "packet sent...");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
