///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 30.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the structures of data for COM board of TAMWA.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_COM_STRUCTS_H_
#define PWRINSPACE_TANWA_COM_STRUCTS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float vbat;
    bool abort_button;
    uint16_t solenoid_state_fill;
    uint16_t solenoid_state_depr;
    float pressure_1;
    float pressure_2;
    float pressure_3;
    float pressure_4;
    float temperature_1;
    float temperature_2;
    bool igniter_cont_1;
    bool igniter_cont_2;
} com_data_t;

typedef struct {
    uint16_t solenoid_state_oxy;
    uint16_t solenoid_state_fuel;
    uint16_t solenoid_state_vent;
    bool arm_state;
    bool abort_button;
    uint32_t time_to_start;
} com_liquid_data_t;

#endif // PWRINSPACE_TANWA_COM_STRUCTS_H_