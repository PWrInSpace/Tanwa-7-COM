///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "measure_task.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "TANWA_config.h"
#include "TANWA_data.h"

#include "now.h"

#include "mcu_gpio_config.h"
#include "state_machine_config.h"

#include "can_commands.h"
#include "can_task.h"
#include "timers_config.h"

#include "esp_log.h"

#define TAG "MEASURE_TASK"

#define MEASURE_TASK_STACK_SIZE 4096
#define MEASURE_TASK_PRIORITY 1
#define MEASURE_TASK_CORE 1
#define MEASURE_TASK_DEFAULT_FREQ 500

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

static TaskHandle_t measure_task_handle = NULL;
static SemaphoreHandle_t measure_task_freq_mutex = NULL;
static volatile TickType_t measure_task_freq = MEASURE_TASK_DEFAULT_FREQ;

void run_measure_task(void) {
    measure_task_freq_mutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(measure_task, "measure_task", MEASURE_TASK_STACK_SIZE, NULL, 
                            MEASURE_TASK_PRIORITY, &measure_task_handle, MEASURE_TASK_CORE);
}

void stop_measure_task(void) {
    vTaskDelete(measure_task_handle);
}

void change_measure_task_period(uint32_t period_ms) {
    if (xSemaphoreTake(measure_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
        measure_task_freq = (TickType_t) period_ms;
        xSemaphoreGive(measure_task_freq_mutex);
    }
}

void copy_tanwa_data_to_now_struct(DataToObc *now_struct){
    // Get data from shared memory
    tanwa_data_t tanwa_data = tanwa_data_read();
    // Copy data to now struct
    now_struct->tanWaState = tanwa_data.state;
    now_struct->pressureSensor = (uint16_t) tanwa_data.com_data.pressure_1;
    now_struct->solenoid_fill = tanwa_data.com_data.solenoid_state_fill;
    now_struct->solenoid_depr = tanwa_data.com_data.solenoid_state_depr;
    now_struct->abortButton = 0; // TODO: Implement abort button state
    now_struct->igniterContinouity_1 = tanwa_data.com_data.igniter_cont_1;
    now_struct->igniterContinouity_2 = tanwa_data.com_data.igniter_cont_2;
    now_struct->hxRequest_RCK = tanwa_data.can_hx_rocket_status.request;
    now_struct->hxRequest_TANK = tanwa_data.can_hx_oxidizer_status.request;
    now_struct->vbat = tanwa_data.com_data.vbat;
    now_struct->motorState_1 = tanwa_data.com_data.solenoid_state_fill;
    now_struct->motorState_2 = tanwa_data.com_data.solenoid_state_depr;
    now_struct->motorState_3 = tanwa_data.can_fac_status.motor_state_1;
    now_struct->motorState_4 = tanwa_data.can_fac_status.motor_state_2;
    now_struct->rocketWeight_blink = 0; // TODO: Implement
    now_struct->rocketWeight_temp = tanwa_data.can_hx_rocket_status.temperature;
    now_struct->tankWeight_blink = 0; // TODO: Implement
    now_struct->tankWeight_temp = tanwa_data.can_hx_oxidizer_status.temperature;
    now_struct->rocketWeight_val = tanwa_data.can_hx_rocket_data.weight;
    now_struct->tankWeight_val = tanwa_data.can_hx_oxidizer_data.weight;
    now_struct->rocketWeightRaw_val = tanwa_data.can_hx_rocket_data.weight_raw;
    now_struct->tankWeightRaw_val = tanwa_data.can_hx_oxidizer_data.weight_raw;
    now_struct->interface_rck = 0; // TODO: Implement
    now_struct->interface_tank = 0; // TODO: Implement
    now_struct->interface_mcu = 0; // TODO: Implement
}

void measure_task(void* pvParameters) {
    ESP_LOGI(TAG, "### Measurement task started ###");

    TickType_t last_wake_time;
    TickType_t local_freq;

    solenoid_driver_valve_state_t sol_fill, sol_depr;
    igniter_continuity_t ign_cont_1, ign_cont_2;
    float vbat, temp[2], pressure[4];

    // Initialise the xLastWakeTime variable with the current time.
    last_wake_time = xTaskGetTickCount();

    while (1) {
        // Wait for the next cycle.
        if (xSemaphoreTake(measure_task_freq_mutex, (TickType_t) 10) == pdTRUE) {
            local_freq = measure_task_freq;
            xSemaphoreGive(measure_task_freq_mutex);
        
            vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(local_freq));

            // Update state from state machine
            tanwa_data_update_state((uint8_t) state_machine_get_current_state());

            com_data_t com_data;

            // Measure battery voltage
            TANWA_get_vbat(&vbat);
            // ESP_LOGI(TAG, "Battery voltage: %.2f", vbat);
            com_data.vbat = vbat;

            // Check solenoid states
            solenoid_driver_valve_get_state(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL, &sol_fill);
            solenoid_driver_valve_get_state(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR, &sol_depr);
            com_data.solenoid_state_fill = sol_fill;
            com_data.solenoid_state_depr = sol_depr;

            // Measure pressure
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &pressure[0]);
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure[1]);
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_3, &pressure[2]);
            pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_4, &pressure[3]);
            // ESP_LOGI(TAG, "Pressure: %.2f", pressure);
            com_data.pressure_1 = pressure[0];
            com_data.pressure_2 = pressure[1];
            com_data.pressure_3 = pressure[2];
            com_data.pressure_4 = pressure[3];

            // Measure temperature
            for (int i = 0; i < 2; ++i) {
                tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[i]), &temp[i]);
            }
            // ESP_LOGI(TAG, "Temperature sensors 1: %.2f, 2: %.2f", temp[0], temp[1]);
            com_data.temperature_1 = temp[0];
            com_data.temperature_2 = temp[1];

            // Check igniter continuity
            igniter_check_continuity(&(TANWA_hardware.igniter[0]), &ign_cont_1);
            igniter_check_continuity(&(TANWA_hardware.igniter[1]), &ign_cont_2);
            com_data.igniter_cont_1 = (bool) ign_cont_1;
            com_data.igniter_cont_2 = (bool) ign_cont_2;

            // Update COM data
            tanwa_data_update_com_data(&com_data);

            // Oxidizer board status
            twai_message_t hx_oxi_stat = CAN_HX_OXI_GET_STATUS();
            can_task_add_message_with_rx(&hx_oxi_stat);

            // Oxidizer weight measurement
            twai_message_t hx_oxi_mess = CAN_HX_OXI_GET_DATA();
            can_task_add_message_with_rx(&hx_oxi_mess);

            // Rocket board status 
            twai_message_t hx_rck_stat = CAN_HX_RCK_GET_STATUS();
            can_task_add_message_with_rx(&hx_rck_stat);

            // Rocket weight measurement
            twai_message_t hx_rck_mess = CAN_HX_RCK_GET_DATA();
            can_task_add_message_with_rx(&hx_rck_mess);

            // Filling arm status
            twai_message_t filling_arm_stat = CAN_FAC_GET_STATUS();
            can_task_add_message_with_rx(&filling_arm_stat);

            // Termo control status
            twai_message_t termo_stat = CAN_TERMO_GET_STATUS();
            can_task_add_message_with_rx(&termo_stat);

            // Termo control data
            twai_message_t termo_mess = CAN_TERMO_GET_DATA();
            can_task_add_message_with_rx(&termo_mess);

            // Update esp now data structure
            DataToObc now_data_struct;
            copy_tanwa_data_to_now_struct(&now_data_struct);
            esp_now_send(adress_obc, (uint8_t*) &now_data_struct, sizeof(DataToObc));
        }
    }
    vTaskDelete(NULL);
}