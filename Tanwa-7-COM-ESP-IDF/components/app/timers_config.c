// Copyright 2022 PWrInSpace, Kuba
#include "timers_config.h"

#include "sd_task.h"
#include "TANWA_data.h"

#include "esp_log.h"

#define TAG "TIMERS"

void on_sd_timer(void *arg){
    tanwa_data_t tanwa_data = tanwa_data_read();
    if (SDT_send_data(&tanwa_data, sizeof(tanwa_data)) == false) {
        ESP_LOGE(TAG, "Error while sending data to sd card");
    }
}

bool initialize_timers(void) {
    sys_timer_t timers[] = {
    {.timer_id = TIMER_SD_DATA,             .timer_callback_fnc = on_sd_timer,              .timer_arg = NULL},
};
    return sys_timer_init(timers, sizeof(timers) / sizeof(timers[0]));
}