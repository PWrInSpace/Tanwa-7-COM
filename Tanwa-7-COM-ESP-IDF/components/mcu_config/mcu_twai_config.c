// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_twai_config.h"

#define TWAI_TAG "TWAI_CONFIG"

esp_err_t twai_init(mcu_twai_config_t *config, twai_general_config_t *g_config,
                    twai_timing_config_t *t_config,
                    twai_filter_config_t *f_config) {
  esp_err_t err;
  err = twai_driver_install(g_config, t_config, f_config);
  if (err != ESP_OK) {
    ESP_LOGE(TWAI_TAG, "TWAI driver install error");
    return err;
  }
  err = twai_start();
  if (err != ESP_OK) {
    ESP_LOGE(TWAI_TAG, "TWAI start error");
    return err;
  }
  return ESP_OK;
}

twai_message_t twai_compose_self_test_message(uint32_t id,
                                              uint8_t data_length_code,
                                              uint8_t *data) {
  twai_message_t msg = {
      .identifier = id, .data_length_code = data_length_code, .self = 1};
  for (int i = 0; i < data_length_code; i++) {
    msg.data[i] = data[i];
  }
  return msg;
}
