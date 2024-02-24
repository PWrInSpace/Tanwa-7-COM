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
#include "igniter_driver.h"

#include "led_state_display.h"
#include "pressure_driver.h"
#include "solenoid_driver.h"

// TANWA hardware
typedef struct {
    led_struct_t esp_led;
    tmp1075_struct_t tmp1075[2];
    mcp23018_struct_t mcp23018;
    ads1115_struct_t ads1115;
    pca9574_struct_t pca9574;
    igniter_struct_t igniter[2];
} TANWA_hardware_t;

// TANWA utilities 
typedef struct {
    led_state_display_struct_t led_state_display;
    pressure_driver_struct_t pressure_driver;
    solenoid_driver_struct_t solenoid_driver;
} TANWA_utility_t;

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

esp_err_t TANWA_mcu_config_init();

esp_err_t TANWA_hardware_init();

esp_err_t TANWA_utility_init();

// esp_err_t TANWA_get_hardware(TANWA_hardware_t** hardware);

// esp_err_t TANWA_get_utility(TANWA_utility_t** utility);

#endif /* PWRINSPACE_TANWA_CONFIG_H_*/