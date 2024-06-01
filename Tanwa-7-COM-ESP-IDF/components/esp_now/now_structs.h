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
  NO_CHANGE = 0xff //DO NOT USE, ONLY FOR REQUEST PURPOSE
} States;

typedef struct DataToObc {
    uint8_t tanWaState;
    uint16_t pressureSensor;
    uint16_t solenoid_fill; //pin pa0 10 pin stm adc
    uint16_t solenoid_depr; // pin pa2 12 pin stm adc
    bool abortButton : 1;
    bool igniterContinouity_1;
    bool igniterContinouity_2;
    uint8_t hxRequest_RCK;
    uint8_t hxRequest_TANK;
    float vbat;
    uint8_t motorState_1;
    uint8_t motorState_2;
    uint8_t motorState_3;
    uint8_t motorState_4;
    int rocketWeight_blink;
    float rocketWeight_temp;
    int tankWeight_blink;
    float tankWeight_temp;
    float rocketWeight_val;
    float tankWeight_val;
    uint32_t rocketWeightRaw_val;
    uint32_t tankWeightRaw_val;
    bool interface_rck;
    bool interface_tank;
    bool interface_mcu;
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