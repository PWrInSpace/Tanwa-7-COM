// Copyright 2022 Pwr in Space, Krzysztof Gliwiński

#include <string.h>

#include "esp_log.h"
#include "flash_api.h"
#include "unity.h"

#define READ_FILE_NAME (const char*)"/spiffs/test_data.txt"
#define WRITE_FILE_NAME (const char*)"/spiffs/test_write_data.txt"
#define EXPECTED_DATA \
  (const char*)"Test text, nothing to find here...Widzisz mie?\
  \x09<\xD4\xF9\xCA?\xF0\xB1\xCB?\x88>\xCB?\xF0\xB1\xCB?\xC1\x95\x04\x82"
#define WRITE_DATA (const char*)"Lorem ipsum\0"

#define TAG "Flash test"

TEST_CASE("Flash init and read file test", "[flash]") {
  TEST_ASSERT_EQUAL(FLASH_OK, FLASH_init(1));
}

TEST_CASE("Flash read file test", "[flash]") {
  char readData[40];
  FlashResult res;
  res = FLASH_read_all_data(READ_FILE_NAME, readData, 1000000);
  TEST_ASSERT_EQUAL(FLASH_OK, res);
  // ESP_LOGI(TAG, "%s", readData);

  TEST_ASSERT_EQUAL_STRING(EXPECTED_DATA, readData);
}

TEST_CASE("Flash deinit test", "[flash]") {
  TEST_ASSERT_EQUAL(FLASH_OK, FLASH_deinit());
}

