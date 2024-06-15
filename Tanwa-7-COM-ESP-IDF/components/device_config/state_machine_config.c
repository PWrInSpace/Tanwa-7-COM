///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "state_machine_config.h"

#include <stddef.h>

#include "esp_log.h"
#include "sdkconfig.h"

#include "TANWA_config.h"

#define TAG "SMC"

extern TANWA_utility_t TANWA_utility;

static void on_init(void *arg) {
    ESP_LOGI(TAG, "ON INIT");
}

static void on_idle(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_IDLE);
    ESP_LOGI(TAG, "ON IDLE");
}

static void on_recovery_arm(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_ARMED);
    ESP_LOGI(TAG, "ON ARM");
}

static void on_fueling(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_FUELING);
    ESP_LOGI(TAG, "ON FUELING");
}

static void on_armed_to_launch(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_ARMED_TO_LAUNCH);
    ESP_LOGI(TAG, "ON ARMED TO LAUNCH");
}

static void on_ready_to_lauch(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_RDY_TO_LAUNCH);
    ESP_LOGI(TAG, "ON READY_TO_LAUNCH");
}

static void on_countdown(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_COUTDOWN);
    ESP_LOGI(TAG, "ON COUNTDOWN");
}

static void on_flight(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_FLIGHT);
    ESP_LOGI(TAG, "----> ON FLIGHT <----");
}

static void on_first_stage_recovery(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_FIRST_STAGE);
    ESP_LOGI(TAG, "ON FIRST STAGE RECOVERY");
}

static void on_second_stage_recovery(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_SECOND_STAGE);
    ESP_LOGI(TAG, "ON FIRST STAGE RECOVERY");
}

static void on_ground(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_ON_GROUND);
    ESP_LOGI(TAG, "ON GROUND");
}

static void on_hold(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_HOLD);
    ESP_LOGI(TAG, "ON HOLD");
}

static void on_abort(void *arg) {
    led_state_display_state_update(&TANWA_utility.led_state_display, LED_STATE_DISPLAY_STATE_ABORT);
    ESP_LOGI(TAG, "ON ABORT");
}

static state_config_t states_cfg[] = {
    {INIT, on_init, NULL},
    {IDLE, on_idle, NULL},
    {RECOVERY_ARM, on_recovery_arm, NULL},
    {FUELING, on_fueling, NULL},
    {ARMED_TO_LAUNCH, on_armed_to_launch, NULL},
    {RDY_TO_LAUNCH, on_ready_to_lauch, NULL},
    {COUNTDOWN, on_countdown, NULL},
    {FLIGHT, on_flight, NULL},
    {FIRST_STAGE_RECOVERY, on_first_stage_recovery, NULL},
    {SECOND_STAGE_RECOVERY, on_second_stage_recovery, NULL},
    {ON_GROUND, on_ground, NULL},
    {HOLD, on_hold, NULL},
    {ABORT, on_abort, NULL},
};

bool initialize_state_machine(void) {
    ESP_LOGI(TAG, "Initializing state machine");
    state_machine_task_cfg_t task_cfg = {
        .stack_depth = CONFIG_SM_TASK_STACK_DEPTH,
        .core_id = CONFIG_SM_TASK_CORE_ID,
        .priority = CONFIG_SM_TASK_PRIORITY,
    };

    state_machine_status_t status = STATE_MACHINE_OK;
    state_machine_init();
    status |= state_machine_set_states(states_cfg, sizeof(states_cfg)/sizeof(states_cfg[0]));
    status |= state_machine_run(&task_cfg);
    return status == STATE_MACHINE_OK ? true : false;
}

void get_state_text(int32_t state, char *text) {
    switch (state) {
        case INIT:
            strcpy(text, "INIT");
            break;
        case IDLE:
            strcpy(text, "IDLE");
            break;
        case RECOVERY_ARM:
            strcpy(text, "RECOVERY_ARM");
            break;
        case FUELING:
            strcpy(text, "FUELING");
            break;
        case ARMED_TO_LAUNCH:
            strcpy(text, "ARMED_TO_LAUNCH");
            break;
        case RDY_TO_LAUNCH:
            strcpy(text, "RDY_TO_LAUNCH");
            break;
        case COUNTDOWN:
            strcpy(text, "COUNTDOWN");
            break;
        case FLIGHT:
            strcpy(text, "FLIGHT");
            break;
        case FIRST_STAGE_RECOVERY:
            strcpy(text, "FIRST_STAGE_RECOVERY");
            break;
        case SECOND_STAGE_RECOVERY:
            strcpy(text, "SECOND_STAGE_RECOVERY");
            break;
        case ON_GROUND:
            strcpy(text, "ON_GROUND");
            break;
        case HOLD:
            strcpy(text, "HOLD");
            break;
        case ABORT:
            strcpy(text, "ABORT");
            break;
        default:
            strcpy(text, "UNKNOWN");
            break;
    }
}