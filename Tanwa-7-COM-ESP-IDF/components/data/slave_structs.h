///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 30.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the structures of data for the slave TANWA submodules.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_SLAVE_STRUCTS_H_
#define PWRINSPACE_TANWA_SLAVE_STRUCTS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool hx_rocket: 1;
    bool hx_oxidizer: 1;
    bool fac: 1;
    bool flc: 1;
    bool termo: 1;
} can_connected_slaves_t;

///===-----------------------------------------------------------------------------------------===//
/// HX Rocket
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint16_t status;
    uint8_t request;
    int16_t temperature;
} can_hx_rocket_status_t;

typedef struct {
    float weight;
    uint32_t weight_raw;
} can_hx_rocket_data_t;

///===-----------------------------------------------------------------------------------------===//
/// HX Oxidizer
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint16_t status;
    uint8_t request;
    int16_t temperature;
} can_hx_oxidizer_status_t;

typedef struct {
    float weight;
    uint32_t weight_raw;
} can_hx_oxidizer_data_t;

///===-----------------------------------------------------------------------------------------===//
/// FAC - Filling Arm Controller
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint16_t status;
    uint8_t request;
    uint8_t motor_state_1;
    uint8_t motor_state_2;
    uint8_t limit_switch_1;
    uint8_t limit_switch_2;
    uint8_t limit_switch_3;
    uint8_t limit_switch_4;
    uint8_t servo_state_1;
    uint8_t servo_state_2;
} can_fac_status_t;

///===-----------------------------------------------------------------------------------------===//
/// FLC - Filling Controller
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint16_t status;
    uint8_t request;
    int16_t temperature;
} can_flc_status_t;

typedef struct {
    int16_t temperature_1;
    int16_t temperature_2;
    int16_t temperature_3;
    int16_t temperature_4;
} can_flc_data_t;

///===-----------------------------------------------------------------------------------------===//
/// TERMO - Oxidizer Tank Temperature Controller
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint16_t status;
    uint8_t request;
} can_termo_status_t;

typedef struct {
    float pressure;
    float temperature;
} can_termo_data_t;

#endif // PWRINSPACE_SLAVE_STRUCTS_H_