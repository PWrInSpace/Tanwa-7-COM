// Copyright 2022 PWrInSpace, Kuba
#ifndef TIMER_CFG_H
#define TIMER_CFG_H

#include "system_timer.h"
#include "stdbool.h"

typedef enum {
    TIMER_SD_DATA = 0,
} timers_id_def;

/**
 * @brief Initialize timers
 * 
 * @return true :D
 * @return false :C
 */
bool initialize_timers(void);

#endif