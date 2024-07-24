///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 24.07.2024 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_LIQUID_STRUCTS_H_
#define PWRINSPACE_LIQUID_STRUCTS_H_

#include <stdbool.h>

#include "slave_structs.h"
#include "com_structs.h"

///===-----------------------------------------------------------------------------------------===//
/// Main Valve
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    float pressure_1;
    float pressure_2;
} now_main_valve_pressure_data_t;

typedef struct {
    float temperature_1;
    float temperature_2;
} now_main_valve_temperature_data_t;

///===-----------------------------------------------------------------------------------------===//
/// Liquid data
///===-----------------------------------------------------------------------------------------===//

typedef struct {
    uint8_t state;

    //main valve
    now_main_valve_pressure_data_t pressure_data;
    now_main_valve_temperature_data_t temperature_data;
    // flc
    can_flc_data_t can_flc_data;
    // hx rocket
    can_hx_oxidizer_data_t can_hx_oxidizer_data;
    // hx oxidizer
    can_hx_rocket_data_t can_hx_rocket_data;
    // termo
    can_termo_data_t now_termo_data;
    // fac
    can_fac_status_t can_fac_status;
    // com
    com_liquid_data_t com_liquid_data;
} liquid_data_t;

///===-----------------------------------------------------------------------------------------===//
/// UPDATE FUNCTIONS
///===-----------------------------------------------------------------------------------------===//

void liquid_data_init(void);

void liquid_data_update_state(uint8_t state);

void liquid_data_update_pressure_data(now_main_valve_pressure_data_t *data);

void liquid_data_update_temperature_data(now_main_valve_temperature_data_t *data);

void liquid_data_update_can_flc_data(can_flc_data_t *data);

void liquid_data_update_can_hx_oxidizer_data(can_hx_oxidizer_data_t *data);

void liquid_data_update_can_hx_rocket_data(can_hx_rocket_data_t *data);

void liquid_data_update_now_termo_data(can_termo_data_t *data);

void liquid_data_update_can_fac_status(can_fac_status_t *data);

void liquid_data_update_com_liquid_data(com_liquid_data_t *data);


///===-----------------------------------------------------------------------------------------===//
/// READ FUNCTIONS
///===-----------------------------------------------------------------------------------------===//

liquid_data_t liquid_data_read(void);

now_main_valve_pressure_data_t liquid_data_read_pressure_data(void);

now_main_valve_temperature_data_t liquid_data_read_temperature_data(void);

can_flc_data_t liquid_data_read_can_flc_data(void);

can_hx_oxidizer_data_t liquid_data_read_can_hx_oxidizer_data(void);

can_hx_rocket_data_t liquid_data_read_can_hx_rocket_data(void);

can_termo_data_t liquid_data_read_now_termo_data(void);

can_fac_status_t liquid_data_read_can_fac_status(void);

com_liquid_data_t liquid_data_read_com_liquid_data(void);

#endif // PWRINSPACE_LIQUID_STRUCTS_H_