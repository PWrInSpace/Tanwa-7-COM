///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_STATE_MACHINE_CONFIG_H_
#define PWRINSPACE_STATE_MACHINE_CONFIG_H_

#include "state_machine.h"

#include "stdbool.h"

typedef enum {
    INIT = 0,
    IDLE,
    RECOVERY_ARM,
    FUELING,
    ARMED_TO_LAUNCH,
    RDY_TO_LAUNCH,
    COUNTDOWN,
    FLIGHT,
    FIRST_STAGE_RECOVERY,
    SECOND_STAGE_RECOVERY,
    ON_GROUND,
    HOLD,
    ABORT,
} states_t;

/**
 * @brief Initialize state machine task
 * 
 * @return true :D
 * @return false :C
 */
bool initialize_state_machine(void);

#endif /* PWRINSPACE_STATE_MACHINE_CONFIG_H_*/