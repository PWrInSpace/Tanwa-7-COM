// Copyright 2022 Pwr in Space, Krzysztof Gliwiński

#include <string.h>

#include "esp_log.h"
#include "flash_nvs.h"
#include "unity.h"

#define TEST_KEY_ZERO (const char*)"test_key_zero"
#define TEST_KEY_FULL (const char*)"test_key_full"
#define TEST_KEY_RAND (const char*)"test_key_random"
#define TEST_VAL_ZERO (uint8_t)0x00
#define TEST_VAL_FULL (uint8_t)0xFF
#define TEST_VAL_RAND (uint8_t)0x69

#define TAG "Flash nvs test"

TEST_CASE("Flash_nvs init test", "[flash_nvs]") {
    TEST_ASSERT_EQUAL(NVS_OK, NVS_init());
}

TEST_CASE("Flash_nvs write and read test", "[flash_nvs]") {
    TEST_ASSERT_EQUAL(NVS_OK, NVS_write_uint8(TEST_KEY_ZERO, TEST_VAL_ZERO));
    TEST_ASSERT_EQUAL(NVS_OK, NVS_write_uint8(TEST_KEY_FULL, TEST_VAL_FULL));
    TEST_ASSERT_EQUAL(NVS_OK, NVS_write_uint8(TEST_KEY_RAND, TEST_VAL_RAND));

    uint8_t readValZero, readValFull, readValRand;

    TEST_ASSERT_EQUAL(NVS_OK, NVS_read_uint8(TEST_KEY_ZERO, &readValZero));
    TEST_ASSERT_EQUAL(NVS_OK, NVS_read_uint8(TEST_KEY_FULL, &readValFull));
    TEST_ASSERT_EQUAL(NVS_OK, NVS_read_uint8(TEST_KEY_RAND, &readValRand));

    TEST_ASSERT_EQUAL(TEST_VAL_ZERO, readValZero);
    TEST_ASSERT_EQUAL(TEST_VAL_FULL, readValFull);
    TEST_ASSERT_EQUAL(TEST_VAL_RAND, readValRand);
}

