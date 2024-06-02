///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 02.06.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the ESP-NOW commands IDs and functions to parse messages 
/// received from the rocket Onboard Computer.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_NOW_COMMANDS_H_
#define PWRINSPACE_TANWA_NOW_COMMANDS_H_

#include <stdint.h>
#include <stdbool.h>

///===-----------------------------------------------------------------------------------------===//
// ESP-NOW commands IDs
///===-----------------------------------------------------------------------------------------===//

typedef enum {
    // State commands 
    NOW_STATE_CHANGE = 0x00,
    NOW_ABORT = 0x01,
    NOW_HOLD_IN = 0x02,
    NOW_HOLD_OUT = 0x03,
    // Filling process commands
    NOW_FILL = 0x10,
    NOW_FILL_TIME = 0x15,
    NOW_DEPR = 0x20,
    NOW_QD = 0x30,
    // Firing commands
    NOW_SOFT_ARM = 0x04,
    NOW_SOFT_DISARM = 0x05,
    NOW_FIRE = 0x88,
    // Restart commands
    NOW_SOFT_RESTART_RCK = 0x61,
    NOW_SOFT_RESTART_OXI = 0x62,
    NOW_SOFT_RESTART_ESP = 0x06,
    // Rocket weight calibration commands
    NOW_CALIBRATE_RCK = 0x40,
    NOW_TARE_RCK = 0x41,
    NOW_SET_CAL_FACTOR_RCK = 0x42,
    NOW_SET_OFFSET_RCK = 0x43,
    // Oxidizer weight calibration commands
    NOW_CALIBRATE_OXI = 0x50,
    NOW_TARE_OXI = 0x51,
    NOW_SET_CAL_FACTOR_OXI = 0x52,
    NOW_SET_OFFSET_OXI = 0x53,
    // PLSS command
    NOW_PLSS = 0x60,
    // LoRa config commands
    NOW_LORA_FREQ = 0x80,
    NOW_LORA_TIME = 0x81
} esp_now_command_t;

typedef enum {
    NOW_VALVE_OPEN = 0x01,
    NOW_VALVE_CLOSE = 0x00
} esp_now_valve_cmd_t;

typedef enum {
    NOW_QD_PUSH = 0x01,
    NOW_QD_STOP = 0x22,
    NOW_QD_PULL = 0x00
} esp_now_qd_cmd_t;

///===-----------------------------------------------------------------------------------------===//
// ESP-NOW message parsing
///===-----------------------------------------------------------------------------------------===//

void tanwa_state_change(uint8_t state);

void tanwa_abort(void);

void tanwa_hold_in(void);

void tanwa_hold_out(void);

void tanwa_fill(uint8_t valve_cmd);

void tanwa_fill_time(uint16_t open_time);

void tanwa_depr(uint8_t valve_cmd);

void tanwa_qd_1(uint8_t qd_cmd);

void tanwa_qd_2(uint8_t qd_cmd);

void tanwa_soft_arm(void);

void tanwa_soft_disarm(void);

void tanwa_fire(void);

void tanwa_soft_restart_esp(void);

void tanwa_soft_restart_rck(void);

void tanwa_soft_restart_oxi(void);

#endif /* PWRINSPACE_TANWA_NOW_COMMANDS_H_ */