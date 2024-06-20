// Copyright 2022 PWrInSpace, Kuba
#include "timers_config.h"

#include "buzzer_driver.h"

#include "TANWA_config.h"
#include "TANWA_data.h"

#include "mcu_gpio_config.h"
#include "state_machine_config.h"

#include "sd_task.h"

#include "esp_log.h"

#define TAG "TIMERS"

void on_sd_timer(void *arg){
    tanwa_data_t tanwa_data = tanwa_data_read();
    if (SDT_send_data(&tanwa_data, sizeof(tanwa_data)) == false) {
        ESP_LOGE(TAG, "Error while sending data to sd card");
    }
}

void on_buzzer_timer(void *arg){
    buzzer_driver_status_t ret;
    ret = buzzer_set_state(&TANWA_hardware.buzzer, BUZZER_STATE_ON);
    if (ret != BUZZER_OK) {
        ESP_LOGE(TAG, "Failed to set buzzer state");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    ret = buzzer_set_state(&TANWA_hardware.buzzer, BUZZER_STATE_OFF);
    if (ret != BUZZER_OK) {
        ESP_LOGE(TAG, "Failed to set buzzer state");
        return;
    }
}

void on_abort_button_timer(void *arg){
    // Check the state of the button pin
    uint8_t level;
    _mcu_gpio_get_level(ABORT_GPIO_INDEX, &level);
    if (level == 0) {
        ESP_LOGW(TAG, "ABORT BUTTON PRESSED!");
        // Handle the button press event (e.g., send a message to a task)
        state_machine_force_change_state(ABORT);
    }
    // Re-enable the interrupt
    gpio_intr_enable(ABORT_GPIO);
}

bool initialize_timers(void) {
    sys_timer_t timers[] = {
    {.timer_id = TIMER_SD_DATA, .timer_callback_fnc = on_sd_timer, .timer_arg = NULL},
    {.timer_id = TIMER_BUZZER, .timer_callback_fnc = on_buzzer_timer, .timer_arg = NULL},
    {.timer_id = TIMER_ABORT_BUTTON, .timer_callback_fnc = on_abort_button_timer, .timer_arg = NULL}
    };
    return sys_timer_init(timers, sizeof(timers) / sizeof(timers[0]));
}

bool buzzer_timer_start(uint32_t period_ms) {
    return sys_timer_start(TIMER_BUZZER, period_ms, TIMER_TYPE_PERIODIC);
}

bool buzzer_timer_change_period(uint32_t period_ms) {
    if (!sys_timer_stop(TIMER_BUZZER)) {
        ESP_LOGE(TAG, "Failed to stop buzzer timer");
        return false;
    }
    return sys_timer_start(TIMER_BUZZER, period_ms, TIMER_TYPE_PERIODIC);
}

bool abort_button_timer_start_once(uint32_t period_ms) {
    return sys_timer_start(TIMER_ABORT_BUTTON, period_ms, TIMER_TYPE_ONE_SHOT);
}