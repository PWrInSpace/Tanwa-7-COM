// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "lora_esp32_config.h"

static mcu_spi_config_t* spi_config;

/*!
 * \file lora_esp32_config.c
 * \brief ESP32 lora configuration
 */

// #define CONFIG_CS_GPIO GPIO_NUM_5
// #define CONFIG_SCK_GPIO GPIO_NUM_18
// #define CONFIG_MOSI_GPIO GPIO_NUM_23
// #define CONFIG_MISO_GPIO GPIO_NUM_19
// #define CONFIG_RST_GPIO GPIO_NUM_4

#define TAG "LORA"

static struct {
  uint8_t rst_gpio_num;
  uint8_t cs_gpio_num;
  uint8_t d0_gpio_num;
} lora_pins;

void lora_esp32_config_mount_spi_config(mcu_spi_config_t* _spi_config) {
  spi_config = _spi_config;
}

esp_err_t lora_esp32_config_deinit(void){
  return spi_deinit(spi_config);
}

esp_err_t _lora_spi_and_pins_init(gpio_num_t _rst_gpio_num,
                                  gpio_num_t _cs_gpio_num,
                                  gpio_num_t _d0_gpio_num) {
  esp_err_t ret;
  ret = spi_init(spi_config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize SPI bus.");
    return false;
  }
  lora_pins.rst_gpio_num = _rst_gpio_num;
  lora_pins.cs_gpio_num = _cs_gpio_num;
  lora_pins.d0_gpio_num = _d0_gpio_num;

  // Configure gpio
  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << lora_pins.rst_gpio_num),
      .pull_down_en = 0,
      .pull_up_en = 0,
  };
  ret = gpio_config(&io_conf);

  gpio_config_t io_conf2 = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << lora_pins.cs_gpio_num),
      .pull_down_en = 0,
      .pull_up_en = 1,
  };
  ret |= gpio_config(&io_conf2);

  // TODO(Glibus): check if interrupt is initiated correctly, implement
  // interrupt
  gpio_config_t io_conf3 = {
      .intr_type = GPIO_INTR_LOW_LEVEL,
      .mode = GPIO_MODE_INPUT,
      .pin_bit_mask = (1ULL << lora_pins.d0_gpio_num),
      .pull_down_en = 0,
      .pull_up_en = 1,
  };

  ret |= gpio_config(&io_conf3);

  gpio_set_level(lora_pins.rst_gpio_num, 0);

  ESP_LOGI(TAG, "GPIOs configured.");
  return ret;
}

bool _lora_SPI_transmit(uint8_t _in[2], uint8_t _out[2]) {
  spi_transaction_t t = {.flags = 0,
                         .length = 8 * sizeof(uint8_t) * 2,
                         .tx_buffer = _out,
                         .rx_buffer = _in};

  gpio_set_level(lora_pins.cs_gpio_num, 0);
  // TODO(Gliwus): implement thread safe SPI (also with SD card and flash)
  spi_device_transmit(spi_config->spi_handle, &t);
  gpio_set_level(lora_pins.cs_gpio_num, 1);
  return true;
}

void _lora_delay(size_t _ms) { vTaskDelay(pdMS_TO_TICKS(_ms)); }

bool _lora_GPIO_set_level(uint8_t _gpio_num, uint32_t _level) {
  return gpio_set_level(_gpio_num, _level) == ESP_OK ? true : false;
}

void _lora_log(const char* info) { ESP_LOGI(TAG, "%s", info); }
