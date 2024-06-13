///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 02.06.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "now_commands.h"

#include "esp_log.h"

#include "state_machine_config.h"
#include "TANWA_config.h"

#include "can_commands.h"
#include "can_task.h"

#define TAG "NOW_COMMANDS"

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

void tanwa_state_change(uint8_t state) {
    if (state_machine_get_current_state() == ABORT) {
        ESP_LOGW(TAG, "SM | System in ABORT state");
        return;
    }
    state_machine_status_t sm_status = state_machine_change_state((state_t)state);
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
    if (valve_cmd == NOW_VALVE_OPEN) {
        sol_status = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
    } else if (valve_cmd == NOW_VALVE_CLOSE) {
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
    vTaskDelay(pdMS_TO_TICKS(time));
    sol_status = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
    if (sol_status != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "SOL | Solenoid driver fill error | %d", (uint8_t)sol_status);
    }
}

void tanwa_depr(uint8_t valve_cmd) {
    solenoid_driver_status_t sol_status = SOLENOID_DRIVER_OK;
    if (valve_cmd == NOW_VALVE_OPEN) {
        sol_status = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
    } else if (valve_cmd == NOW_VALVE_CLOSE){
        sol_status = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
    } else {
        ESP_LOGE(TAG, "SOL | Invalid depr valve command | %d", valve_cmd);
    }
    if (sol_status != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "SOL | Solenoid driver depr error | %d", (uint8_t)sol_status);
    }
}

void tanwa_qd_1(uint8_t qd_cmd) {
    // ToDo: send command to FAC vid CAN to push/pull quick disconnect
    if (qd_cmd == NOW_QD_PUSH) {
        // ToDo: send push command to FAC
    } else if (qd_cmd == NOW_QD_STOP) {
        // ToDo: send stop command to FAC
    } else if (qd_cmd == NOW_QD_PULL) {
        // ToDo: send pull command to FAC
    } else {
        ESP_LOGE(TAG, "QD | Invalid command | %d", qd_cmd);
    }
}

void tanwa_qd_2(uint8_t qd_cmd) {
    // ToDo: send command to FAC vid CAN to push/pull quick disconnect
    if (qd_cmd == NOW_QD_PUSH) {
        // ToDo: send push command to FAC
    } else if (qd_cmd == NOW_QD_STOP) {
        // ToDo: send stop command to FAC
    } else if (qd_cmd == NOW_QD_PULL) {
        // ToDo: send pull command to FAC
    } else {
        ESP_LOGE(TAG, "QD | Invalid command | %d", qd_cmd);
    }
}

void tanwa_soft_arm(void) {
    igniter_status_t ign_status = IGNITER_OK;
    ign_status = igniter_arm(&(TANWA_hardware.igniter[0]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter arm error | %d", (uint8_t)ign_status);
    }
    ign_status = igniter_arm(&(TANWA_hardware.igniter[1]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter arm error | %d", (uint8_t)ign_status);
    }
}

void tanwa_soft_disarm(void) {
    igniter_status_t ign_status = IGNITER_OK;
    ign_status = igniter_disarm(&(TANWA_hardware.igniter[0]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter disarm error | %d", (uint8_t)ign_status);
    }
    ign_status = igniter_disarm(&(TANWA_hardware.igniter[1]));
    if (ign_status != IGNITER_OK) {
        ESP_LOGE(TAG, "IGN | Igniter disarm error | %d", (uint8_t)ign_status);
    }
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
}

void tanwa_soft_restart_esp(void) {
    esp_restart();
}

void tanwa_soft_restart_rck(void) {
    // Send soft restart command to HX RCK
    const twai_message_t hx_oxi_mess = CAN_HX_RCK_SOFT_RESET();
    if (twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_soft_restart_oxi(void) {
    // Send soft restart command to HX OXI
    const twai_message_t hx_oxi_mess = CAN_HX_OXI_SOFT_RESET();
    if (twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_calibrate_rck(float weight) {
    // Send calibrate command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_CALIBRATE();
    memcpy(hx_rck_mess.data, &weight, sizeof(float));
    if (twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_tare_rck(void) {
    // Send tare command to HX RCK
    const twai_message_t hx_rck_mess = CAN_HX_RCK_TARE();
    if (twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_set_cal_factor_rck(float cal_factor) {
    // Send set cal factor command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_SET_CALIBRATION_FACTOR();
    memcpy(hx_rck_mess.data, &cal_factor, sizeof(float));
    if (twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_set_offset_rck(float offset) {
    // Send set offset command to HX RCK
    twai_message_t hx_rck_mess = CAN_HX_RCK_SET_OFFSET();
    memcpy(hx_rck_mess.data, &offset, sizeof(float));
    if (twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_calibrate_oxi(float weight) {
    // Send calibrate command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_CALIBRATE();
    memcpy(hx_oxi_mess.data, &weight, sizeof(float));
    if (twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_tare_oxi(void) {
    // Send tare command to HX OXI
    const twai_message_t hx_oxi_mess = CAN_HX_OXI_TARE();
    if (twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_set_cal_factor_oxi(float cal_factor) {
    // Send set cal factor command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_SET_CALIBRATION_FACTOR();
    memcpy(hx_oxi_mess.data, &cal_factor, sizeof(float));
    if (twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}

void tanwa_set_offset_oxi(float offset) {
    // Send set offset command to HX OXI
    twai_message_t hx_oxi_mess = CAN_HX_OXI_SET_OFFSET();
    memcpy(hx_oxi_mess.data, &offset, sizeof(float));
    if (twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100)) == ESP_OK) {
        can_task_add_rx_counter();
        change_can_task_period(100U);
    }
}