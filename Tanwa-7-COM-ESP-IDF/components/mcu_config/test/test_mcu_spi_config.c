// Copyright 2023 PWrInSpace, Krzysztof Gliwiński

#include "mcu_spi_config.h"
#include "sdkconfig.h"
#include "unity.h"

static mcu_spi_config_t spi_config = MCU_SPI_DEFAULT_CONFIG();

TEST_CASE("MCU SPI Init", "[MCU_SPI]") {
  TEST_ASSERT_EQUAL(ESP_OK, spi_init(&spi_config));
}

TEST_CASE("MCU SPI Deinit", "[MCU_SPI]") {
  TEST_ASSERT_EQUAL(ESP_OK, spi_deinit(&spi_config));
}
