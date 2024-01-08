// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "led_driver.h"
#include "sdkconfig.h"
#include "unity.h"

#define LEDC_FREQ_HZ 5000
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_LOW_SPEED_MODE
#define MAX_DUTY 8192  // 2**13

led_driver_t driver = {
    .ledc_mode = LEDC_HS_MODE,
    .led_gpio_num = CONFIG_LED_K_R,
    .ledc_channel_num = LEDC_CHANNEL_0,
    .ledc_timer_num = LEDC_HS_TIMER,
    .duty = 0,
    .max_duty = MAX_DUTY,
    .toggle = LED_OFF,
};

TEST_CASE("LED driver init test", "[LED]") {
  TEST_ASSERT_EQUAL(ESP_OK,
                    led_driver_init(&driver));
}

TEST_CASE("LED driver set correct duty cycle", "[LED]") {
  TEST_ASSERT_EQUAL(ESP_OK, led_update_duty_cycle(&driver, 0));
  TEST_ASSERT_EQUAL(0, driver.duty);
  TEST_ASSERT_EQUAL(ESP_OK, led_update_duty_cycle(&driver, MAX_DUTY / 2));
  TEST_ASSERT_EQUAL(MAX_DUTY / 2, driver.duty);
  TEST_ASSERT_EQUAL(ESP_OK, led_update_duty_cycle(&driver, MAX_DUTY));
  TEST_ASSERT_EQUAL(MAX_DUTY, driver.duty);
}

TEST_CASE("LED driver set wrong duty cycle", "[LED]") {
  TEST_ASSERT_EQUAL(ESP_FAIL, led_update_duty_cycle(&driver, MAX_DUTY + 1));
  TEST_ASSERT_EQUAL(ESP_FAIL, led_update_duty_cycle(&driver, -1));
}

TEST_CASE("LED driver set correct toggle", "[LED]") {
  led_toggle(&driver, LED_OFF);
  TEST_ASSERT_EQUAL(LED_OFF, driver.toggle);
  TEST_ASSERT_EQUAL(ESP_OK, led_toggle(&driver, LED_ON));
  TEST_ASSERT_EQUAL(LED_ON, driver.toggle);
  TEST_ASSERT_EQUAL(ESP_OK, led_toggle(&driver, LED_OFF));
}

TEST_CASE("LED driver set wrong toggle", "[LED]") {
  TEST_ASSERT_EQUAL(ESP_FAIL, led_toggle(&driver, 2));
  TEST_ASSERT_EQUAL(LED_OFF, driver.toggle);
  TEST_ASSERT_EQUAL(ESP_FAIL, led_toggle(&driver, -1));
  TEST_ASSERT_EQUAL(LED_OFF, driver.toggle);
}
