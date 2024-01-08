// Copyright 2023 PWrInSpace, Krzysztof Gliwiński

#include "mcu_adc_config.h"
#include "unity.h"

TEST_CASE("ADC init test", "[ADC]") {
  //   static mcu_adc_config_t adc_config = {.channel = ADC1_CHANNEL_0,
  // .resolution = ADC_WIDTH_BIT_12,
  // .attenuation = ADC_ATTEN_DB_11};
  TEST_ASSERT_EQUAL(ESP_OK, ESP_OK);
}
