// Copyright 2022 PWrInSpace, Kuba
#ifndef COMMANDS_CONFIG_H
#define COMMANDS_CONFIG_H

#include "commands.h"

#define LORA_DEV_ID 0x04

/**
 * @brief Initialzie lora commands
 * 
 * @return true :D
 * @return false :C
 */
bool lora_cmd_init(void);

/**
 * @brief Process received command from lora
 *
 * @param lora_dev_id lora_dev_id
 * @param dev_id receiver_device_id
 * @param message message
 * @return true :D
 * @return false :C
 */
bool lora_cmd_process_command(cmd_lora_dev_id lora_dev_id,
                              cmd_sys_dev_id dev_id,
                              cmd_message_t *message);

typedef enum {
    TANWA_FILL = 0x10,
    TANWA_FILL_TIME = 0x15,
    TANWA_DEPR = 0x20,
    TANWA_QD = 0x30,
    TANWA_QD_2 = 0x31,
    TANWA_RESTART_ESP_RCK = 0x61,
    TANWA_RESTART_ESP_BTL = 0x62,
    TANWA_SOFT_RESTART_ESP = 0x06,
    TANWA_SOFT_RESTART_STM = 0x07,
    TANWA_CALIBRATE_RCK = 0x40,
    TANWA_TARE_RCK = 0x41,
    TANWA_SET_CAL_FACTOR_RCK = 0x42,
    TANWA_SET_OFFSET_RCK = 0x43,
    TANWA_CALIBRATE_TANK = 0x50,
    TANWA_TARE_TANK = 0x51,
    TANWA_SET_CAL_FACTOR_TANK = 0x52,
    TANWA_SET_OFFSET_TANK = 0x53,
    TANWA_SOFT_ARM = 0x04,
    TANWA_SOFT_DISARM = 0x05,
    TANWA_ITF_RCK = 0x44,
    TANWA_ITF_TANK = 0x54,
} tanwa_commands_t;

#endif