///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_NOW_STRUCTS_H_
#define PWRINSPACE_TANWA_NOW_STRUCTS_H_

#include <stdbool.h>
#include <stdint.h>

#define ANSWER_WAIT_MS 750
#define MIN_SLEEP_TIME_MS 2000
#define SLEEP_TIME_MS 30000

// IMPORTANT! To set for each module and state:
typedef enum {
    INIT_PERIOD             = SLEEP_TIME_MS,
    IDLE_PERIOD             = SLEEP_TIME_MS,
    RECOVERY_ARM_PERIOD     = SLEEP_TIME_MS,
    FUELING_PERIOD          = 500,
    ARMED_TO_LAUNCH_PERIOD  = 1000,
    RDY_TO_LAUNCH_PERIOD    = 1000,
    COUNTDOWN_PERIOD        = 500,
    FLIGHT_PERIOD           = 10,
    FIRST_STAGE_REC_PERIOD  = 250,
    SECOND_STAGE_REC_PERIOD = 500,
    ON_GROUND_PERIOD        = SLEEP_TIME_MS,
    HOLD_PERIOD             = 1000,
    ABORT_PERIOD            = SLEEP_TIME_MS
} Periods;
// IMPORTANT END!

typedef struct DataToObc {
    float vbat;
    uint8_t tanWaState;
    float rocketWeight_val;
    float tankWeight_val;
    float tank_temp;
    float fill_temp;
    float preFill_pres;
    float postFill_pres;
    float tank_pres;
    bool canHxBtl_con : 1;
    bool canHxRck_con : 1;
    bool canFac_con : 1;
    bool canFlc_con : 1;
    bool canTermo_con : 1;
    bool igniterContinouity_1 : 1;
    bool igniterContinouity_2 : 1;
    bool limitSwitch_1 : 1;
    bool limitSwitch_2 : 1;
    bool fillState : 1;
    bool deprState : 1;
    uint8_t facMotorState_1;
    uint8_t facMotorState_2;
    bool coolingState : 1;
    bool heatingState : 1;
    bool abortButton : 1;
} DataToObc;

typedef struct {
    uint32_t commandNum;
    int32_t commandArg;
} DataFromObc;

// typedef struct {
//     DataToObc dataToObc;
//     DataFromObc dataFromObc;
//     uint8_t obcState;
//     uint16_t stateTimes[13];
// } ModuleData;

// volatile ModuleData module_data;

#endif