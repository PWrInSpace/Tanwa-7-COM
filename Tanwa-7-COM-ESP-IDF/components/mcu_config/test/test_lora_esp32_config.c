// Copyright 2023 PWrInSpace, Krzysztof Gliwiński

#include "lora.h"
#include "lora_esp32_config.h"
#include "sdkconfig.h"
#include "unity.h"

lora_struct_t lora = {._spi_transmit = _lora_SPI_transmit,
                      ._delay = _lora_delay,
                      ._gpio_set_level = _lora_GPIO_set_level,
                      .log = _lora_log,
                      .rst_gpio_num = CONFIG_LORA_RS,
                      .cs_gpio_num = CONFIG_LORA_CS,
                      .d0_gpio_num = CONFIG_LORA_D0,
                      .implicit_header = 0,
                      .frequency = 0};

static mcu_spi_config_t spi = {
    .host_id = SPI2_HOST,
    .bus_config = {.miso_io_num = CONFIG_SPI_MISO,
                   .mosi_io_num = CONFIG_SPI_MOSI,
                   .sclk_io_num = CONFIG_SPI_SCK,
                   .quadwp_io_num = -1,
                   .quadhd_io_num = -1,
                   .max_transfer_sz = 4000},
    .dev_config = {.clock_speed_hz = 9000000,
                   .mode = 0,
                   .spics_io_num = -1,
                   .queue_size = 1,
                   .flags = 0,
                   .pre_cb = NULL},
    .spi_init_flag = false,
};

const char* lora_packet = "TEST;";

TEST_CASE("Lora Init test", "[LORA]") {
  esp_err_t ret;
  lora_err_t lora_ret;
  ret = spi_init(&spi);
  TEST_ASSERT_EQUAL(ret, ESP_OK);
  lora_esp32_config_mount_spi_config(&spi);
  ret = _lora_spi_and_pins_init(lora.rst_gpio_num, lora.cs_gpio_num,
                                lora.d0_gpio_num);
  TEST_ASSERT_EQUAL(ret, ESP_OK);
  lora_ret = lora_init(&lora);
  TEST_ASSERT_EQUAL(lora_ret, LORA_OK);
  vTaskDelay(pdMS_TO_TICKS(100));

  lora_ret = lora_set_frequency(&lora, 867e6);
  TEST_ASSERT_EQUAL(lora_ret, LORA_OK);
  lora_ret = lora_set_bandwidth(&lora, LORA_BW_250_kHz);
  TEST_ASSERT_EQUAL(lora_ret, LORA_OK);
  lora_ret = lora_disable_crc(&lora);
  TEST_ASSERT_EQUAL(lora_ret, LORA_OK);
}

TEST_CASE("Lora read register test", "[LORA]") {
  int16_t read_val_one = lora_read_reg(&lora, 0x0d);
  int16_t read_val_two = lora_read_reg(&lora, 0x0c);
  TEST_ASSERT_EQUAL_HEX16(0x009d, read_val_one);
  TEST_ASSERT_EQUAL_HEX16(0x001e, read_val_two);
  lora_reset(&lora);
}

TEST_CASE("Lora send packet", "[LORA]") {
  lora_err_t ret =
      lora_send_packet(&lora, (uint8_t*)lora_packet, strlen(lora_packet));
  TEST_ASSERT_EQUAL(ret, LORA_OK);
}

TEST_CASE("Lora SPI deinit", "[LORA]") {
  esp_err_t ret = lora_esp32_config_deinit();
  TEST_ASSERT_EQUAL(ret, ESP_OK);
}
