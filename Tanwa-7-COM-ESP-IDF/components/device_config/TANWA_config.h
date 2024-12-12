///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_TANWA_CONFIG_H_
#define PWRINSPACE_TANWA_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#include "led_driver.h"
#include "tmp1075.h"
#include "mcp23018.h"
#include "ads1115.h"
#include "pca9574.h"
#include "sdcard.h"
#include "igniter_driver.h"
#include "buzzer_driver.h"

#include "led_state_display.h"
#include "pressure_driver.h"
#include "solenoid_driver.h"

#include "valve_control.h"

#include "esp_now.h"
#include "lora.h"

#define SERVO_CLOSE_POSITION 150U
#define SERVO_OPEN_POSITION 40U

#define POT_SERVO_OPEN_VALUE 0U
#define POT_SERVO_CLOSE_VALUE 860U

#define CLOSE_STATE 0U
#define OPEN_STATE 1U
#define BETWEEN_STATE 2U

// TANWA hardware
typedef struct {
    led_struct_t esp_led;
    tmp1075_struct_t tmp1075[2];
    mcp23018_struct_t mcp23018;
    ads1115_struct_t ads1115;
    pca9574_struct_t pca9574;
    igniter_struct_t igniter[2];
    buzzer_struct_t buzzer;
} TANWA_hardware_t;

// TANWA utilities 
typedef struct {
    led_state_display_struct_t led_state_display;
    pressure_driver_struct_t pressure_driver;
    solenoid_driver_struct_t solenoid_driver;
    Valve_Servo_t servo_valve[2];
} TANWA_utility_t;

// LoRa communication
typedef lora_struct_t TANWA_lora_t;

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;
extern TANWA_lora_t TANWA_lora;

esp_err_t TANWA_mcu_config_init();

esp_err_t TANWA_hardware_init();

esp_err_t TANWA_utility_init();

esp_err_t TANWA_esp_now_init();

// esp_err_t TANWA_get_hardware(TANWA_hardware_t** hardware);

// esp_err_t TANWA_get_utility(TANWA_utility_t** utility);

esp_err_t TANWA_get_vbat(float* vbat);

#endif /* PWRINSPACE_TANWA_CONFIG_H_*/