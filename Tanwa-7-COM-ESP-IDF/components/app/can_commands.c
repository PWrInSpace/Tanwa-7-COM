///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 01.06.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "can_commands.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "TANWA_data.h"
#include "can_task.h"

#define TAG "CAN_COMMANDS"

#define CAN_HX_STATUS_POS 0
#define CAN_HX_STATUS_REQUEST_POS 2
#define CAN_HX_STATUS_TEMP_POS 6

#define CAN_HX_DATA_WEIGHT_POS 0
#define CAN_HX_DATA_WEIGHT_RAW_POS 4

#define CAN_FAC_STATUS_POS 0
#define CAN_FAC_STATUS_REQUEST_POS 2
#define CAN_FAC_STATUS_MOTOR_POS 3
#define CAN_FAC_STATUS_LIMIT_1_2_POS 5
#define CAN_FAC_STATUS_LIMIT_3_4_POS 6
#define CAN_FAC_STATUS_SERVO_POS 4
#define CAN_FAC_DATA_OFFSET 4
#define CAN_FAC_DATA_MASK 0x0F


#define CAN_FLC_STATUS_POS 0
#define CAN_FLC_STATUS_REQUEST_POS 2
#define CAN_FLC_STATUS_TEMP_POS 6

#define CAN_FLC_DATA_TEMP_1_POS 0
#define CAN_FLC_DATA_TEMP_2_POS 2
#define CAN_FLC_DATA_TEMP_3_POS 4
#define CAN_FLC_DATA_TEMP_4_POS 6

#define CAN_FLC_DATA_PRESSURE_1_POS 0
#define CAN_FLC_DATA_PRESSURE_2_POS 2
#define CAN_FLC_DATA_PRESSURE_3_POS 4
#define CAN_FLC_DATA_PRESSURE_4_POS 6

#define CAN_TERMO_STATUS_POS 0
#define CAN_TERMO_STATUS_REQUEST_POS 1
#define CAN_TERMO_HEATING_STATUS_POS 2
#define CAN_TERMO_COOLING_STATUS_POS 3
#define CAN_TERMO_MAX_PRESSURE_POS 4
#define CAN_TERMO_MIN_PRESSURE_POS 5

#define CAN_TERMO_DATA_PRESSURE_POS 0
#define CAN_TERMO_DATA_TEMP_POS 4

void parse_can_hx_rck_status(twai_message_t rx_message) {
    // update hx rck status
    can_hx_rocket_status_t hx_rck_status = {
        .status = *((uint16_t*)rx_message.data + CAN_HX_STATUS_POS),
        .request = *((uint8_t*)(rx_message.data + CAN_HX_STATUS_REQUEST_POS)),
        .temperature = *((int16_t*)(rx_message.data + CAN_HX_STATUS_TEMP_POS)),
    };
    // ESP_LOGI(TAG, "HX RCK status: status: %d, request: %d, temperature: %d", hx_rck_status.status, hx_rck_status.request, hx_rck_status.temperature);
    tanwa_data_update_can_hx_rocket_status(&hx_rck_status);
    if (hx_rck_status.request == CAN_REQ_SOFT_RESET) {
        twai_message_t hx_oxi_mess = CAN_HX_RCK_SOFT_RESET();
        can_task_add_message(&hx_oxi_mess);
    }
}

void parse_can_hx_rck_data(twai_message_t rx_message) {
    // update hx oxi data
    can_hx_rocket_data_t hx_rck_data = {
        .weight = *((float*)(rx_message.data + CAN_HX_DATA_WEIGHT_POS)),
        .weight_raw = *((uint32_t*)(rx_message.data + CAN_HX_DATA_WEIGHT_RAW_POS)),
    };
    // ESP_LOGI(TAG, "HX RCK data: weight: %.2f, weight raw: %d", hx_rck_data.weight, hx_rck_data.weight_raw);
    tanwa_data_update_can_hx_rocket_data(&hx_rck_data);
}

void parse_can_hx_oxi_status(twai_message_t rx_message) {
    // update hx oxi status
    can_hx_oxidizer_status_t hx_oxi_status = {
        .status = *((uint16_t*)(rx_message.data + CAN_HX_STATUS_POS)),
        .request = *((uint8_t*)(rx_message.data + CAN_HX_STATUS_REQUEST_POS)),
        .temperature = *((int16_t*)(rx_message.data + CAN_HX_STATUS_TEMP_POS)),
    };
    // ESP_LOGI(TAG, "HX OXI status: status: %d, request: %d, temperature: %d", hx_oxi_status.status, hx_oxi_status.request, hx_oxi_status.temperature);
    tanwa_data_update_can_hx_oxidizer_status(&hx_oxi_status);
    if (hx_oxi_status.request == CAN_REQ_SOFT_RESET) {
        twai_message_t hx_oxi_mess = CAN_HX_OXI_SOFT_RESET();
        can_task_add_message(&hx_oxi_mess);
    }
}

void parse_can_hx_oxi_data(twai_message_t rx_message) {
    // update hx oxi data
    //ESP_LOGI(TAG, "DLC: %d", rx_message.data_length_code);
    can_hx_oxidizer_data_t hx_rck_data = {
        .weight = *((float*)(rx_message.data + CAN_HX_DATA_WEIGHT_POS)),
        .weight_raw = *((uint32_t*)(rx_message.data + CAN_HX_DATA_WEIGHT_RAW_POS)),
    };
    // ESP_LOGI(TAG, "HX OXI data: weight: %.2f, weight raw: %d", hx_rck_data.weight, hx_rck_data.weight_raw);
    tanwa_data_update_can_hx_oxidizer_data(&hx_rck_data);
}

void parse_can_fac_status(twai_message_t rx_message) {
    // update fac status
    can_fac_status_t fac_status = {
        .status = *((uint16_t*)rx_message.data + CAN_FAC_STATUS_POS),
        .request = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_REQUEST_POS)),
        .motor_state_1 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_MOTOR_POS)) >> CAN_FAC_DATA_OFFSET,
        .motor_state_2 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_MOTOR_POS)) & CAN_FAC_DATA_MASK,
        .limit_switch_1 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_LIMIT_1_2_POS)) >> CAN_FAC_DATA_OFFSET,
        .limit_switch_2 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_LIMIT_1_2_POS)) & CAN_FAC_DATA_MASK,
        .limit_switch_3 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_LIMIT_3_4_POS)) >> CAN_FAC_DATA_OFFSET,
        .limit_switch_4 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_LIMIT_3_4_POS)) & CAN_FAC_DATA_MASK,
        .servo_state_1 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_SERVO_POS)) >> CAN_FAC_DATA_OFFSET,
        .servo_state_2 = *((uint8_t*)(rx_message.data + CAN_FAC_STATUS_SERVO_POS)) & CAN_FAC_DATA_MASK,
    };
    ESP_LOGI(TAG, "FAC status: status: %d, request: %d, motor state 1: %d, motor state 2: %d, limit switch 1: %d, limit switch 2: %d, SERVO 1: %d, SERVO 2: %d", fac_status.status, fac_status.request, fac_status.motor_state_1, fac_status.motor_state_2, fac_status.limit_switch_1, fac_status.limit_switch_2, fac_status.servo_state_1, fac_status.servo_state_2);
    tanwa_data_update_can_fac_status(&fac_status);
    // if (fac_status.request == CAN_REQ_SOFT_RESET) {
    //     twai_message_t fac_mess = CAN_FAC_SOFT_RESET();
    //     can_task_add_message(&fac_mess);
    // }
}

void parse_can_flc_status(twai_message_t rx_message) {
    // update flc status
    can_flc_status_t flc_status = {
        .status = *((uint16_t*)rx_message.data + CAN_FLC_STATUS_POS),
        .request = *((uint8_t*)(rx_message.data + CAN_FLC_STATUS_REQUEST_POS)),
        .temperature = *((int16_t*)(rx_message.data + CAN_FLC_STATUS_TEMP_POS)),
    };
    //ESP_LOGI(TAG, "FLC status: status: %d, request: %d, temperature: %d", flc_status.status, flc_status.request, flc_status.temperature);
    tanwa_data_update_can_flc_status(&flc_status);
    if (flc_status.request == CAN_REQ_SOFT_RESET) {
        twai_message_t flc_mess = CAN_FLC_SOFT_RESET();
        can_task_add_message(&flc_mess);
    }
}

void parse_can_flc_data(twai_message_t rx_message) {
    // update flc data
    can_flc_data_t flc_data = {
        .temperature_1 = *((int16_t*)rx_message.data + CAN_FLC_DATA_TEMP_1_POS),
        .temperature_2 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_TEMP_2_POS)),
        .temperature_3 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_TEMP_3_POS)),
        .temperature_4 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_TEMP_4_POS)),
    };
    //ESP_LOGI(TAG, "FLC data: temperature 1: %d, temperature 2: %d, temperature 3: %d, temperature 4: %d", flc_data.temperature_1, flc_data.temperature_2, flc_data.temperature_3, flc_data.temperature_4);
    tanwa_data_update_can_flc_data(&flc_data);
}

void parse_can_flc_pressure_data(twai_message_t rx_message) {
    // update flc pressure data
    can_flc_pressure_data_t flc_pressure_data = {
        .pressure_1 = *((int16_t*)rx_message.data + CAN_FLC_DATA_PRESSURE_1_POS),
        .pressure_2 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_PRESSURE_2_POS)),
        .pressure_3 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_PRESSURE_3_POS)),
        .pressure_4 = *((int16_t*)(rx_message.data + CAN_FLC_DATA_PRESSURE_4_POS)),
    };
    //ESP_LOGI(TAG, "FLC pressure data: pressure 1: %.2f, pressure 2: %.2f", flc_pressure_data.pressure_1, flc_pressure_data.pressure_2);
    tanwa_data_update_can_flc_pressure_data(&flc_pressure_data);
}

void parse_can_termo_status(twai_message_t rx_message) {
    // update termo status
    can_termo_status_t termo_status = {
        .status = *((uint16_t*)rx_message.data + CAN_TERMO_STATUS_POS),
        .request = *((uint8_t*)(rx_message.data + CAN_TERMO_STATUS_REQUEST_POS)),
        .cooling_status = *((uint8_t*)(rx_message.data + CAN_TERMO_COOLING_STATUS_POS)),
        .heating_status = *((uint8_t*)(rx_message.data + CAN_TERMO_HEATING_STATUS_POS)),
        .max_pressure = *((uint8_t*)(rx_message.data + CAN_TERMO_MAX_PRESSURE_POS)),
        .min_pressure = *((uint8_t*)(rx_message.data + CAN_TERMO_MIN_PRESSURE_POS)),
    };
    //ESP_LOGI(TAG, "TERMO status: status: %d, request: %d", termo_status.status, termo_status.request);
    tanwa_data_update_can_termo_status(&termo_status);
    // if (termo_status.request == CAN_REQ_SOFT_RESET) {
    //     twai_message_t termo_mess = CAN_TERMO_SOFT_RESET();
    //     can_task_add_message(&termo_mess);
    // }
}

void parse_can_termo_data(twai_message_t rx_message) {
    // update termo data
    can_termo_data_t termo_data = {
        .pressure = *((float*)rx_message.data + CAN_TERMO_DATA_PRESSURE_POS),
        .temperature = *((float*)(rx_message.data + CAN_TERMO_DATA_TEMP_POS)),
    };
    //ESP_LOGI(TAG, "TERMO data: pressure: %.2f, temperature: %d", termo_data.pressure, termo_data.temperature);
    tanwa_data_update_can_termo_data(&termo_data);
}