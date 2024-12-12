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

#define LORA_DEV_ID_ALL 0x00
#define LORA_DEV_ID_ALL_SUDO 0x01
#define LORA_DEV_ID_TANWA 0x04
#define LORA_DEV_ID_TANWA_SUDO 0x05
///===-----------------------------------------------------------------------------------------===//
// ESP-NOW & LORA commands IDs
///===-----------------------------------------------------------------------------------------===//

typedef enum {
    // State commands 
    CMD_STATE_CHANGE = 0x00,
    CMD_ABORT = 0x01,
    CMD_HOLD_IN = 0x02,
    CMD_HOLD_OUT = 0x03,
    CMD_LORA_TRANSMIT_F = 0x10,
    CMD_LORA_TRANSMIT_T = 0x11,
    CMD_COUNTDOWN = 0x12,
    CMD_IGNITERS_TIME = 0x13,
    CMD_FLASH = 0x14,
    CMD_SEND_SETTINGS = 0x15,
    CMD_RESET_ERRORS = 0x16,
    CMD_FLASH_FORMAT = 0x17,
    CMD_FUEL_INIT_TIME = 0x18,
    CMD_FUEL_FULL_TIME = 0x19,
    CMD_OXI_FULL_TIME = 0x20,
    CMD_OXI_INIT_ANGLE = 0x21,
    CMD_FUEL_INIT_ANGLE = 0x22,

    CMD_RESET = 0x80,

    CMD_DISCONNECT_TIMER = 0xFF,

    CMD_FUEL_CLOSE = 0x23,
    CMD_FUEL_OPEN = 0x24,
    CMD_FUEL_OPEN_ANGLE = 0x25,
    CMD_OXI_CLOSE = 0x26,
    CMD_OXI_OPEN = 0x27,
    CMD_OXI_OPEN_ANGLE = 0x28,

    CMD_SOFT_ARM = 0x29,
    CMD_SOFT_DISARM = 0x30,
    CMD_RESTART_RCK = 0x31,
    CMD_RESTART_OXI = 0x32,
    CMD_CALIBRATE_RCK = 0x34,
    CMD_TARE_RCK = 0x35,
    CMD_SET_CAL_FACTOR_RCK = 0x36,
    CMD_SET_OFFSET_RCK = 0x37,
    CMD_CALIBRATE_OXI = 0x38,
    CMD_TARE_OXI = 0x39,
    CMD_SET_CAL_FACTOR_OXI = 0x40,
    CMD_SET_OFFSET_OXI = 0x41,
} cmd_command_t;

typedef enum {
    CMD_VALVE_OPEN = 0x01,
    CMD_VALVE_CLOSE = 0x00
} cmd_valve_t;

typedef enum {
    CMD_QD_PUSH = 0x01,
    CMD_QD_STOP = 0x22,
    CMD_QD_PULL = 0x00
} cmd_qd_t;

typedef enum {
    CMD_HEATING_START = 0x01,
    CMD_HEATING_STOP = 0x00
} cmd_termo_t;

///===-----------------------------------------------------------------------------------------===//
// Command message parsing
///===-----------------------------------------------------------------------------------------===//

void tanwa_state_change(int32_t state);

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

void tanwa_calibrate_rck(float weight);

void tanwa_tare_rck(void);

void tanwa_set_cal_factor_rck(float weight);

void tanwa_set_offset_rck(float offset);

void tanwa_calibrate_oxi(float weight);

void tanwa_tare_oxi(void);

void tanwa_set_cal_factor_oxi(float weight);

void tanwa_set_offset_oxi(float offset);

void tanwa_heating(uint8_t heating_cmd);

///===-----------------------------------------------------------------------------------------===//
/// LORA message switch
///===-----------------------------------------------------------------------------------------===//

bool lora_command_parsing(uint32_t lora_id, uint32_t command, int32_t payload);

#endif /* PWRINSPACE_TANWA_NOW_COMMANDS_H_ */