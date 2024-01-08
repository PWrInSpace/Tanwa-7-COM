// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "rgb_led_driver.h"
#include "sdkconfig.h"
#include "unity.h"

#define LEDC_FREQ_HZ 5000
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_LOW_SPEED_MODE
#define MAX_DUTY 8192  // 2**13

static rgb_led_driver_t rgb_led_driver = {
    .led_drv =
        {
            [RED_INDEX] =
                {
                    .ledc_mode = LEDC_HS_MODE,
                    .led_gpio_num = CONFIG_LED_K_R,
                    .ledc_channel_num = LEDC_CHANNEL_0,
                    .ledc_timer_num = LEDC_HS_TIMER,
                    .duty = 0,
                    .max_duty = MAX_DUTY,
                    .toggle = LED_OFF,
                    .inverted = true,
                },
            [GREEN_INDEX] = {.ledc_mode = LEDC_HS_MODE,
                             .led_gpio_num = CONFIG_LED_K_G,
                             .ledc_channel_num = LEDC_CHANNEL_1,
                             .ledc_timer_num = LEDC_HS_TIMER,
                             .duty = 0,
                             .max_duty = MAX_DUTY,
                             .toggle = LED_OFF,
                             .inverted = true},
            [BLUE_INDEX] =
                {
                    .ledc_mode = LEDC_HS_MODE,
                    .led_gpio_num = CONFIG_LED_K_B,
                    .ledc_channel_num = LEDC_CHANNEL_2,
                    .ledc_timer_num = LEDC_HS_TIMER,
                    .duty = 0,
                    .max_duty = MAX_DUTY,
                    .toggle = LED_OFF,
                    .inverted = true,
                },
        },
    .max_duty = MAX_DUTY};

TEST_CASE("RGB LED driver init test", "[RGBLED]") {
  TEST_ASSERT_EQUAL(ESP_OK, rgb_led_driver_init(&rgb_led_driver, LEDC_DUTY_RES,
                                                LEDC_FREQ_HZ));
}

TEST_CASE("RGB LED driver set correct duty cycle", "[RGBLED]") {
  TEST_ASSERT_EQUAL(ESP_OK, rgb_led_update_duty_cycle(&rgb_led_driver,
                                                      (uint32_t[]){0, 0, 0}));
  TEST_ASSERT_EQUAL(0, rgb_led_driver.led_drv[RED_INDEX].duty);
  TEST_ASSERT_EQUAL(0, rgb_led_driver.led_drv[GREEN_INDEX].duty);
  TEST_ASSERT_EQUAL(0, rgb_led_driver.led_drv[BLUE_INDEX].duty);
  TEST_ASSERT_EQUAL(
      ESP_OK, rgb_led_update_duty_cycle(
                  &rgb_led_driver,
                  (uint32_t[]){MAX_DUTY / 2, MAX_DUTY / 2, MAX_DUTY / 2}));
  TEST_ASSERT_EQUAL(
      ESP_OK, rgb_led_update_duty_cycle(
                  &rgb_led_driver, (uint32_t[]){MAX_DUTY, MAX_DUTY, MAX_DUTY}));
  TEST_ASSERT_EQUAL(ESP_OK,
                    rgb_led_update_duty_cycle(&rgb_led_driver,
                                              (uint32_t[]){MAX_DUTY, 69, 0}));
}

TEST_CASE("RGB LED driver set wrong duty cycle", "[RGBLED]") {
  TEST_ASSERT_EQUAL(
      ESP_FAIL, rgb_led_update_duty_cycle(&rgb_led_driver,
                                          (uint32_t[]){MAX_DUTY + 1, 0, 0}));
  TEST_ASSERT_EQUAL(
      ESP_FAIL, rgb_led_update_duty_cycle(&rgb_led_driver,
                                          (uint32_t[]){0, MAX_DUTY + 1, 0}));
  TEST_ASSERT_EQUAL(
      ESP_FAIL, rgb_led_update_duty_cycle(&rgb_led_driver,
                                          (uint32_t[]){0, 0, MAX_DUTY + 1}));
  TEST_ASSERT_EQUAL(
      ESP_FAIL, rgb_led_update_duty_cycle(
                    &rgb_led_driver,
                    (uint32_t[]){MAX_DUTY + 1, MAX_DUTY + 1, MAX_DUTY + 1}));
}

TEST_CASE("RGB LED driver set correct toggle", "[RGBLED]") {
  rgb_led_toggle(&rgb_led_driver, LED_OFF);
  TEST_ASSERT_EQUAL(LED_OFF, rgb_led_driver.led_drv[RED_INDEX].toggle);
  TEST_ASSERT_EQUAL(LED_OFF, rgb_led_driver.led_drv[GREEN_INDEX].toggle);
  TEST_ASSERT_EQUAL(LED_OFF, rgb_led_driver.led_drv[BLUE_INDEX].toggle);
  TEST_ASSERT_EQUAL(ESP_OK, rgb_led_toggle(&rgb_led_driver, LED_ON));
  TEST_ASSERT_EQUAL(LED_ON, rgb_led_driver.led_drv[RED_INDEX].toggle);
  TEST_ASSERT_EQUAL(LED_ON, rgb_led_driver.led_drv[GREEN_INDEX].toggle);
  TEST_ASSERT_EQUAL(LED_ON, rgb_led_driver.led_drv[BLUE_INDEX].toggle);
  TEST_ASSERT_EQUAL(ESP_OK, rgb_led_toggle(&rgb_led_driver, LED_OFF));
}

TEST_CASE("RGB LED driver set wrong toggle", "[RGBLED]") {
  led_state_t led_state = rgb_led_driver.led_drv[RED_INDEX].toggle;
  TEST_ASSERT_EQUAL(ESP_FAIL, rgb_led_toggle(&rgb_led_driver, 2));
  TEST_ASSERT_EQUAL(led_state, rgb_led_driver.led_drv[RED_INDEX].toggle);
}
