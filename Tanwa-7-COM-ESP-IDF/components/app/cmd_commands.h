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
    // Filling process commands
    CMD_FILL = 0x10,
    CMD_FILL_TIME = 0x15,
    CMD_DEPR = 0x20,
    CMD_QD = 0x30,
    // Firing commands
    CMD_SOFT_ARM = 0x04,
    CMD_SOFT_DISARM = 0x05,
    CMD_FIRE = 0x88,
    // Restart commands
    CMD_SOFT_RESTART_RCK = 0x61,
    CMD_SOFT_RESTART_OXI = 0x62,
    CMD_SOFT_RESTART_ESP = 0x06,
    // Rocket weight calibration commands
    CMD_CALIBRATE_RCK = 0x40,
    CMD_TARE_RCK = 0x41,
    CMD_SET_CAL_FACTOR_RCK = 0x42,
    CMD_SET_OFFSET_RCK = 0x43,
    // Oxidizer weight calibration commands
    CMD_CALIBRATE_OXI = 0x50,
    CMD_TARE_OXI = 0x51,
    CMD_SET_CAL_FACTOR_OXI = 0x52,
    CMD_SET_OFFSET_OXI = 0x53,
    // PLSS command
    CMD_PLSS = 0x60,
    // LoRa config commands
    CMD_LORA_FREQ = 0x80,
    CMD_LORA_TIME = 0x81,
    //Bottle heating commands
    CMD_HEATING = 0x47,
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

void lora_command_parsing(uint32_t lora_id, uint32_t command, int32_t payload);

#endif /* PWRINSPACE_TANWA_NOW_COMMANDS_H_ */