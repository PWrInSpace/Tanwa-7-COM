// Copyright 2023 PWrInSpace, Krzysztof Gliwiński

#include "mcu_twai_config.h"
#include "sdkconfig.h"
#include "unity.h"

mcu_twai_config_t twai_config = {.tx_gpio_num = CONFIG_UART_1_RX,
                                 .rx_gpio_num = CONFIG_UART_1_TX,
                                 .mode = TWAI_MODE_NORMAL};

#define MSG_ID 0x555

TEST_CASE("TWAI init test", "[TWAI]") {
  static twai_general_config_t twai_g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      CONFIG_UART_1_RX, CONFIG_UART_1_TX, TWAI_MODE_NORMAL);
  static twai_timing_config_t twai_t_config = TWAI_TIMING_CONFIG_250KBITS();
  static twai_filter_config_t twai_f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  TEST_ASSERT_EQUAL(ESP_OK, twai_init(&twai_config, &twai_g_config,
                                      &twai_t_config, &twai_f_config));
}

TEST_CASE("TWAI self test message test", "[TWAI]") {
  uint8_t data = 0;
  twai_message_t tx_msg = twai_compose_self_test_message(MSG_ID, 1, &data);
  TEST_ASSERT_EQUAL(MSG_ID, tx_msg.identifier);
  TEST_ASSERT_EQUAL(1, tx_msg.data_length_code);
  TEST_ASSERT_EQUAL(1, tx_msg.self);
}
