///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 02.06.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "cmd_commands.h"

#include "esp_log.h"

#include "state_machine_config.h"
#include "TANWA_config.h"

#include "lora_task.h"

#include "can_commands.h"
#include "can_task.h"

#include "settings_mem.h"
#include "mission_timer_config.h"
#include "timers_config.h"
#include "system_timer.h"

#include "TANWA_data.h"

#define TAG "CMD_COMMANDS"

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

void tanwa_state_change(int32_t state) {
    if (state_machine_get_current_state() == ABORT) {
        ESP_LOGW(TAG, "SM | System in ABORT state");
        return;
    }
    state_machine_status_t sm_status = state_machine_change_state(state);
    if (sm_status != STATE_MACHINE_OK) {
        ESP_LOGE(TAG, "SM | State change error | %d", (uint8_t)sm_status);
    }
}

void tanwa_abort(void) {
    state_t curr_state = state_machine_get_current_state();
    state_t prev_state = state_machine_get_previous_state();
    if (curr_state == ABORT) {
        ESP_LOGW(TAG, "SM | Already in ABORT state");
        return;
    }
    if (curr_state > COUNTDOWN && curr_state < HOLD) {
        ESP_LOGW(TAG, "SM | Abort not possible in FLIGHT");
        return;
    }
    state_machine_status_t sm_status = state_machine_force_change_state(ABORT);
    if (sm_status != STATE_MACHINE_OK) {
        ESP_LOGE(TAG, "SM | State force ABORT error | %d", (uint8_t)sm_status);
    }
}

void tanwa_hold_in(void) {
    state_t curr_state = state_machine_get_current_state();
    state_t prev_state = state_machine_get_previous_state();
    if (curr_state == HOLD) {
        ESP_LOGW(TAG, "SM | Already in HOLD state");
        return;
    }
    if (curr_state == ABORT) {
        ESP_LOGW(TAG, "SM | System in ABORT state");
        return;
    }
    if (curr_state > COUNTDOWN && curr_state < HOLD) {
        ESP_LOGW(TAG, "SM | Hold not possible in FLIGHT");
        return;
    }
    state_machine_status_t sm_status = state_machine_force_change_state(HOLD);
    if (sm_status != STATE_MACHINE_OK) {
        ESP_LOGE(TAG, "SM | State force HOLD error | %d", (uint8_t)sm_status);
    }
}

void tanwa_hold_out(void) {
    state_t curr_state = state_machine_get_current_state();
    state_t prev_state = state_machine_get_previous_state();
    if (curr_state != HOLD) {
        ESP_LOGW(TAG, "SM | Not in HOLD state");
        return;
    }
    if (prev_state == COUNTDOWN) {
        state_machine_status_t sm_status = state_machine_force_change_state(RDY_TO_LAUNCH);
        if (sm_status != STATE_MACHINE_OK) {
            ESP_LOGE(TAG, "SM | State force RDY_TO_LAUNCH error | %d", sm_status);
        }
    } else {
        state_machine_status_t sm_status = state_machine_change_to_previous_state(true);
        if (sm_status != STATE_MACHINE_OK) {
            ESP_LOGE(TAG, "SM | State change error | %d",(uint8_t) sm_status);
        }
    }
}

void tanwa_fill(uint8_t valve_cmd) {
    solenoid_driver_status_t sol_status = SOLENOID_DRIVER_OK;
    if (valve_cmd == CMD_VALVE_OPEN) {
        sol_status = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
    } else if (valve_cmd == CMD_VALVE_CLOSE) {
        sol_status = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
    } else {
        ESP_LOGE(TAG, "SOL | Invalid fill valve command | %d", valve_cmd);
    }
    if (sol_status != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "SOL | Solenoid driver fill error | %d", (uint8_t)sol_status);
    }
}

void tanwa_fill_time(uint16_t open_time) {
    solenoid_driver_status_t sol_status = SOLENOID_DRIVER_OK;
    sol_status = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
    if (sol_status != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "SOL | Solenoid driver fill error | %d", sol_status);
    }
    vTaskDelay(pdMS_TO_TICKS(open_time));
    sol_status = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
    if (sol_status != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "SOL | Solenoid driver fill error | %d", (uint8_t)sol_status);
    }
}

void tanwa_depr(uint8_t valve_cmd) {
    solenoid_driver_status_t sol_status = SOLENOID_DRIVER_OK;
    if (valve_cmd == CMD_VALVE_OPEN) {
        sol_status = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
    } else if (valve_cmd == CMD_VALVE_CLOSE){
        sol_status = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
    } else {
        ESP_LOGE(TAG, "SOL | Invalid depr valve command | %d", valve_cmd);
    }
    if (sol_status != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "SOL | Solenoid driver depr error | %d", (uint8_t)sol_status);
    }
}

void tanwa_qd_1(uint8_t qd_cmd) {
    if (qd_cmd == CMD_QD_PUSH) {
        twai_message_t fac_mess = CAN_FAC_QD_PUSH();
        can_task_add_message(&fac_mess);
    } else if (qd_cmd == CMD_QD_STOP) {
        twai_message_t fac_mess = CAN_FAC_QD_STOP();
        can_task_add_message(&fac_mess);
    } else if (qd_cmd == CMD_QD_PULL) {
        twai_message_t fac_mess = CAN_FAC_QD_PULL();
        can_task_add_message(&fac_mess);
    } else {
        ESP_LOGE(TAG, "QD | Invalid command | %d", qd_cmd);
    }
}

void tanwa_qd_2(uint8_t qd_cmd) {
    // ToDo: send command to FAC vid CAN to push/pull quick disconnect
    if (qd_cmd == CMD_QD_PUSH) {
        // ToDo: send push command to FAC
    } else if (qd_cmd == CMD_QD_STOP) {
        // ToDo: send stop command to FAC
    } else if (qd_cmd == CMD_QD_PULL) {
        // ToDo: send pull command to FAC
    } else {
        ESP_LOGE(TAG, "QD 2 | Invalid command | %d", qd_cmd);
    }
}

void tanwa_soft_arm(void) {
    com_liquid_data_t data = tanwa_data_read_com_liquid_data();
    igniter_status_t ign_status = IGNITER_OK;
    ign_status = igniter_arm(&(TANWA_hardware.igniter[0]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter arm error | %d", (uint8_t)ign_status);
    }
    ign_status = igniter_arm(&(TANWA_hardware.igniter[1]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter arm error | %d", (uint8_t)ign_status);
    }

    if(ign_status == IGNITER_OK){
        data.arm_state = true;
    }
    else{
        data.arm_state = false;
    }

    tanwa_data_update_com_liquid_data(&data);
}

void tanwa_soft_disarm(void) {

    com_liquid_data_t data = tanwa_data_read_com_liquid_data();
    igniter_status_t ign_status = IGNITER_OK;
    ign_status = igniter_disarm(&(TANWA_hardware.igniter[0]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter disarm error | %d", (uint8_t)ign_status);
    }
    ign_status = igniter_disarm(&(TANWA_hardware.igniter[1]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter disarm error | %d", (uint8_t)ign_status);
    }

    data.arm_state = IGNITER_OK == ign_status ? false : true;

    tanwa_data_update_com_liquid_data(&data);
}

void tanwa_fire(void) {
    igniter_status_t ign_status = IGNITER_OK;
    ign_status = igniter_fire(&(TANWA_hardware.igniter[0]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter fire error | %d", ign_status);
    }
    ign_status = igniter_fire(&(TANWA_hardware.igniter[1]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter fire error | %d", ign_status);
    }

    if(ign_status == IGNITER_OK){
        com_liquid_data_t data = tanwa_data_read_com_liquid_data();
        data.arm_state = false;
        tanwa_data_update_com_liquid_data(&data);
    }
}

void tanwa_soft_restart_esp(void) {
    esp_restart();
}

void tanwa_soft_restart_rck(void) {
    // Send soft restart command to HX RCK
    twai_message_t hx_oxi_mess = CAN_HX_RCK_SOFT_RESET();
    can_task_add_message(&hx_oxi_mess);
}

void tanwa_soft_restart_oxi(void) {
    // Send soft restart command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_SOFT_RESET();
    can_task_add_message(&hx_oxi_mess);
}

void tanwa_calibrate_rck(float weight) {
    // Send calibrate command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_CALIBRATE();
    memcpy(hx_rck_mess.data, &weight, sizeof(float));
    can_task_add_message(&hx_rck_mess);
}

void tanwa_tare_rck(void) {
    // Send tare command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_TARE();
    can_task_add_message(&hx_rck_mess);
}

void tanwa_set_cal_factor_rck(float cal_factor) {
    // Send set cal factor command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_SET_CALIBRATION_FACTOR();
    memcpy(hx_rck_mess.data, &cal_factor, sizeof(float));
    can_task_add_message(&hx_rck_mess);
}

void tanwa_set_offset_rck(float offset) {
    // Send set offset command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_SET_OFFSET();
    memcpy(hx_rck_mess.data, &offset, sizeof(float));
    can_task_add_message(&hx_rck_mess);
}

void tanwa_calibrate_oxi(float weight) {
    // Send calibrate command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_CALIBRATE();
    memcpy(hx_oxi_mess.data, &weight, sizeof(float));
    can_task_add_message(&hx_oxi_mess);
}

void tanwa_tare_oxi(void) {
    // Send tare command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_TARE();
    can_task_add_message(&hx_oxi_mess);
}

void tanwa_set_cal_factor_oxi(float cal_factor) {
    // Send set cal factor command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_SET_CALIBRATION_FACTOR();
    memcpy(hx_oxi_mess.data, &cal_factor, sizeof(float));
    can_task_add_message(&hx_oxi_mess);
}

void tanwa_set_offset_oxi(float offset) {
    // Send set offset command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_SET_OFFSET();
    memcpy(hx_oxi_mess.data, &offset, sizeof(float));
    can_task_add_message(&hx_oxi_mess);
}

void tanwa_heating(uint8_t heating_cmd) {

    if(heating_cmd == CMD_HEATING_START) {
        twai_message_t termo_mess = CAN_TERMO_HEAT_START();
        can_task_add_message(&termo_mess);
        ESP_LOGI(TAG, "TERM | Heating start");
    } else if(heating_cmd == CMD_HEATING_STOP) {
        twai_message_t termo_mess = CAN_TERMO_HEAT_STOP();
        can_task_add_message(&termo_mess);
        ESP_LOGI(TAG, "TERM | Heating stop");
    } else {
        ESP_LOGE(TAG, "TERM | Invalid heating command | %d", heating_cmd);
        return;
    }
}

bool lora_command_parsing(uint32_t lora_id, uint32_t command, int32_t payload) {
    if (lora_id == LORA_DEV_ID_ALL || lora_id == LORA_DEV_ID_ALL_SUDO || 
        lora_id == LORA_DEV_ID_TANWA || lora_id == LORA_DEV_ID_TANWA_SUDO) { 

        settings_read_all();
        Settings settings = settings_get_all();
        // Check if the command is for this device
        ESP_LOGI(TAG, "LORA | Command for TANWA");
        switch (command) {
            case CMD_STATE_CHANGE: {
                ESP_LOGI(TAG, "LORA | State change | %d", payload);
                state_t state = (state_t) payload;
                char state_text[20];
                get_state_text(state, state_text);
                ESP_LOGI(TAG, "LORA | State change | %s", state_text);
                tanwa_state_change(payload);
                break;
            }
            case CMD_ABORT: {
                ESP_LOGI(TAG, "LORA | Abort");
                tanwa_abort();
                break;
            }
            case CMD_HOLD_IN: {
                ESP_LOGI(TAG, "LORA | Hold in");
                tanwa_hold_in();
                break;
            }
            case CMD_HOLD_OUT: {
                ESP_LOGI(TAG, "LORA | Hold out");
                tanwa_hold_out();
                break;
            }
            case CMD_LORA_TRANSMIT_F: {
                ESP_LOGI(TAG, "LORA | Transmit F");
                lora_change_frequency(payload);
                break;
            }
            case CMD_LORA_TRANSMIT_T: {
                ESP_LOGI(TAG, "LORA | Transmit P");
                //lora_change_period(payload);
                break;
            }
            case CMD_COUNTDOWN: {
                ESP_LOGI(TAG, "LORA | Countdown");
                settings.countdownTime = payload;
                settings_save(SETTINGS_COUNTDOWN_TIME, settings.countdownTime);
                liquid_ignition_test_timer_set_disable_val(settings.countdownTime);
                break;
            }
            case CMD_IGNITERS_TIME: {
                ESP_LOGI(TAG, "LORA | Igniters time");
                settings.ignitTime = payload;
                settings_save(SETTINGS_IGNIT_TIME, settings.ignitTime);
                break;
            }
            case CMD_FLASH: {
                ESP_LOGI(TAG, "LORA | Flash");
                settings.flash_on = payload;
                settings_save(SETTINGS_FLASH_ON, settings.flash_on);
                break;
            }
            case CMD_SEND_SETTINGS: {
                ESP_LOGI(TAG, "LORA | Send settings");
                break;
            }
            case CMD_RESET_ERRORS: {
                ESP_LOGI(TAG, "LORA | Reset errors");
                break;
            }
            case CMD_FLASH_FORMAT: {
                ESP_LOGI(TAG, "LORA | Flash format");
                break;
            }
            case CMD_FUEL_INIT_TIME: {
                ESP_LOGI(TAG, "LORA | Fuel init time");
                settings.fuel_open_time_ms = payload;
                settings_save(FUEL_OPEN_TIME_MS, settings.fuel_open_time_ms);
                break;
            }
            case CMD_FUEL_FULL_TIME: {
                ESP_LOGI(TAG, "LORA | Fuel full time");
                settings.fuel_full_open_time_ms = payload;
                settings_save(FUEL_FULL_OPEN_TIME_MS, settings.fuel_full_open_time_ms);
                break;
            }
            case CMD_OXI_FULL_TIME: {
                ESP_LOGI(TAG, "LORA | Oxidizer full time");
                settings_save(OXIDIZER_FULL_OPEN_TIME_MS, payload);
                break;
            }
            case CMD_OXI_INIT_ANGLE: {
                ESP_LOGI(TAG, "LORA | Oxidizer init angle");
                settings_save(OXIDIZER_VALVE_INITIAL_ANGLE, payload);
                break;
            }
            case CMD_FUEL_INIT_ANGLE: {
                ESP_LOGI(TAG, "LORA | Fuel init angle");
                settings_save(FUEL_VALVE_INITIAL_ANGLE, payload);
                break;
            }
            case CMD_RESET: {
                ESP_LOGI(TAG, "LORA | Reset");
                esp_restart();
                break;
            }
            case CMD_DISCONNECT_TIMER: {
                ESP_LOGI(TAG, "LORA | Disconnect timer");
                if(!sys_timer_restart(TIMER_DISCONNECT, TIMER_DISCONNECT_PERIOD_MS)) {
                    ESP_LOGE(TAG, "LORA | Unable to restart timer");
                }
                break;
            }
            case CMD_FUEL_CLOSE: {
                ESP_LOGI(TAG, "LORA | Fuel close");
                if(!valve_close_servo(&(TANWA_utility.servo_valve[0]))) {
                    ESP_LOGE(TAG, "LORA | Unable to close fuel valve");
                }
                break;
            }
            case CMD_OXI_CLOSE: {
                ESP_LOGI(TAG, "LORA | Oxidizer close");
                if(valve_close_servo(&(TANWA_utility.servo_valve[1]))) {
                    ESP_LOGE(TAG, "LORA | Unable to close oxidizer valve");
                }
                break;
            }
            case CMD_FUEL_OPEN: {
                ESP_LOGI(TAG, "LORA | Fuel open");
                if(valve_open_servo(&(TANWA_utility.servo_valve[0]))) {
                    ESP_LOGE(TAG, "LORA | Unable to open fuel valve");
                }
                break;
            }
            case CMD_OXI_OPEN: {
                ESP_LOGI(TAG, "LORA | Oxidizer open");
                if(valve_open_servo(&(TANWA_utility.servo_valve[1]))) {
                    ESP_LOGE(TAG, "LORA | Unable to open oxidizer valve");
                }
                break;
            }
            case CMD_FUEL_OPEN_ANGLE: {
                ESP_LOGI(TAG, "LORA | Fuel open angle");
                if(valve_move_angle_servo(&(TANWA_utility.servo_valve[0]), payload)) {
                    ESP_LOGE(TAG, "LORA | Unable to move fuel valve");
                }
                break;
            }
            case CMD_OXI_OPEN_ANGLE: {
                ESP_LOGI(TAG, "LORA | Oxidizer open angle");
                if(valve_move_angle_servo(&(TANWA_utility.servo_valve[1]), payload)) {
                    ESP_LOGE(TAG, "LORA | Unable to move oxidizer valve");
                }
                break;
            }
            case CMD_SOFT_ARM: {
                ESP_LOGI(TAG, "LORA | Soft arm");
                tanwa_soft_arm();
                break;
            }
            case CMD_SOFT_DISARM: {
                ESP_LOGI(TAG, "LORA | Soft disarm");
                tanwa_soft_disarm();
                break;
            }
            case CMD_RESTART_RCK: {
                ESP_LOGI(TAG, "LORA | Restart RCK");
                tanwa_soft_restart_rck();
                break;
            }
            case CMD_RESTART_OXI: {
                ESP_LOGI(TAG, "LORA | Restart OXI");
                tanwa_soft_restart_oxi();
                break;
            }
            case CMD_CALIBRATE_RCK: {
                ESP_LOGI(TAG, "LORA | Calibrate RCK");
                tanwa_calibrate_rck((float)payload);
                break;
            }
            case CMD_TARE_RCK: {
                ESP_LOGI(TAG, "LORA | Tare RCK");
                tanwa_tare_rck();
                break;
            }
            case CMD_SET_CAL_FACTOR_RCK: {
                ESP_LOGI(TAG, "LORA | Set cal factor RCK");
                tanwa_set_cal_factor_rck((float)payload);
                break;
            }
            case CMD_SET_OFFSET_RCK: {
                ESP_LOGI(TAG, "LORA | Set offset RCK");
                tanwa_set_offset_rck((float)payload);
                break;
            }
            case CMD_CALIBRATE_OXI: {
                ESP_LOGI(TAG, "LORA | Calibrate OXI");
                tanwa_calibrate_oxi((float)payload);
                break;
            }
            case CMD_TARE_OXI: {
                ESP_LOGI(TAG, "LORA | Tare OXI");
                tanwa_tare_oxi();
                break;
            }
            case CMD_SET_CAL_FACTOR_OXI: {
                ESP_LOGI(TAG, "LORA | Set cal factor OXI");
                tanwa_set_cal_factor_oxi((float)payload);
                break;
            }
            case CMD_SET_OFFSET_OXI: {
                ESP_LOGI(TAG, "LORA | Set offset OXI");
                tanwa_set_offset_oxi((float)payload);
                break;
            }
            default: {
                ESP_LOGI(TAG, "LORA command: %d", command);
                ESP_LOGW(TAG, "LORA | Unknown command");
                return false;
                break;
            }
        }
        return true;
    } else {
        ESP_LOGW(TAG, "LORA | Command for other device");
        return false;
    }
}