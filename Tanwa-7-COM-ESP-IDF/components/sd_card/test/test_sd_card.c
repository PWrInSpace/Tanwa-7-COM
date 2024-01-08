// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "sdcard.h"
#include "sdkconfig.h"
#include "unity.h"

static sdmmc_card_t card;

static const spi_host_device_t host = SPI2_HOST;

static sd_card_t sd_card = {
    .card = &card,
    .spi_host = host,
    .mount_point = SDCARD_MOUNT_POINT,
    .cs_pin = CONFIG_SD_CS,
    .card_detect_pin = CONFIG_SD_CD,
    .mounted = false,
};

static sd_card_config_t sd_card_config = {
    .spi_host = host,
    .mount_point = SDCARD_MOUNT_POINT,
    .cs_pin = CONFIG_SD_CS,
    .cd_pin = CONFIG_SD_CD,
};

const char* path = SD_CREATE_FILE_PREFIX("test.txt");

TEST_CASE("SD card init test", "[SD]") {
  spi_bus_config_t bus = {.miso_io_num = CONFIG_SPI_MISO,
                          .mosi_io_num = CONFIG_SPI_MOSI,
                          .sclk_io_num = CONFIG_SPI_SCK,
                          .quadwp_io_num = -1,
                          .quadhd_io_num = -1,
                          .max_transfer_sz = 4000};
  TEST_ASSERT_EQUAL(ESP_OK, spi_bus_initialize(host, &bus, SDSPI_DEFAULT_DMA));

  // Card detect pin initialization
  gpio_config_t io_conf = {
      .pin_bit_mask = 1ULL << sd_card.card_detect_pin,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  TEST_ASSERT_EQUAL(ESP_OK, gpio_config(&io_conf));

  TEST_ASSERT_EQUAL(true, SD_init(&sd_card, &sd_card_config));
}

TEST_CASE("SD card write test", "[SD]") {
  const char* data = "test";
  size_t length = strlen(data);
  TEST_ASSERT_EQUAL(true, SD_write(&sd_card, path, data, length));
}

TEST_CASE("SD file exists test", "[SD]") {
  TEST_ASSERT_EQUAL(true, SD_file_exists(path));
  TEST_ASSERT_EQUAL(false, SD_file_exists(SD_CREATE_FILE_PREFIX("test_2.txt")));
}

TEST_CASE("SD ok test", "[SD]") { TEST_ASSERT_EQUAL(true, SD_is_ok(&sd_card)); }

TEST_CASE("SD Card detect test", "[SD]") {
  TEST_ASSERT_EQUAL(true, SD_card_detect(&sd_card));
}

TEST_CASE("SD unmount test", "[SD]") {
  TEST_ASSERT_EQUAL(true, SD_unmount(&sd_card));
}

