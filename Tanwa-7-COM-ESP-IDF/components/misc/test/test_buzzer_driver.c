// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "buzzer_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "unity.h"
#define BUZZER_FREQ_HZ 4000
#define BUZZER_DUTY_RES LEDC_TIMER_13_BIT
#define BUZZER_TIMER LEDC_TIMER_1
#define BUZZER_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_MAX_DUTY 8191  // 2**13 - 1
buzzer_driver_t buzzer = {
    .ledc_mode = BUZZER_MODE,
    .gpio_num = CONFIG_BUZZER,
    .ledc_channel_num = LEDC_CHANNEL_3,
    .ledc_timer_num = BUZZER_TIMER,
    .duty = 0,
    .max_duty = BUZZER_MAX_DUTY,
};

TEST_CASE("Buzzer init test", "[BUZZER]") {
  TEST_ASSERT_EQUAL(ESP_OK,
                    buzzer_init(&buzzer, BUZZER_DUTY_RES, BUZZER_FREQ_HZ));
}

TEST_CASE("Buzzer sound test", "[BUZZER]") {
  TEST_ASSERT_EQUAL(ESP_OK,
                    buzzer_update_duty_cycle(&buzzer, BUZZER_MAX_DUTY / 2));
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  TEST_ASSERT_EQUAL(ESP_OK, buzzer_update_duty_cycle(&buzzer, 0));
}
