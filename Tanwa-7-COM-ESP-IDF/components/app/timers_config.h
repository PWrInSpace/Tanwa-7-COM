// Copyright 2022 PWrInSpace, Kuba
#ifndef TIMER_CFG_H
#define TIMER_CFG_H

#include "system_timer.h"
#include "stdbool.h"

typedef enum {
    TIMER_SD_DATA = 0,
    TIMER_BUZZER = 1,
    TIMER_ABORT_BUTTON = 2,
} timers_id_def;

/**
 * @brief Initialize timers
 * 
 * @return true :D
 * @return false :C
 */
bool initialize_timers(void);

bool buzzer_timer_start(uint32_t period_ms);

bool buzzer_timer_change_period(uint32_t period_ms);

bool abort_button_timer_start_once(uint32_t period_ms);

#endif