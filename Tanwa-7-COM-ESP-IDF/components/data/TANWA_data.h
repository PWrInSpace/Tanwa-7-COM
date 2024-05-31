///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 30.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the structures of data for the TANWA.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_DATA_H_
#define PWRINSPACE_TANWA_DATA_H_

#include <stdbool.h>
#include <stdint.h>

#include "com_structs.h"
#include "slave_structs.h"

typedef struct {
    // COM
    com_data_t com_data;
    can_connected_slaves_t can_connected_slaves;
    // HX rocket
    can_hx_rocket_status_t can_hx_rocket_status; 
    can_hx_rocket_data_t can_hx_rocket_data;
    // HX oxidizer
    can_hx_oxidizer_status_t can_hx_oxidizer_status;
    can_hx_oxidizer_data_t can_hx_oxidizer_data;
    // FAC
    can_fac_status_t can_fac_status;
    // FLC
    can_flc_status_t can_flc_status;
    can_flc_data_t can_flc_data;
    // TERMO
    can_termo_status_t can_termo_status;
    can_termo_data_t can_termo_data;
} tanwa_data_t;

bool tanwa_data_init(void);

///===-----------------------------------------------------------------------------------------===//
/// update functions
///===-----------------------------------------------------------------------------------------===//

void tanwa_data_update_com_data(com_data_t *data);

void tanwa_data_update_can_connected_slaves(can_connected_slaves_t *data);

void tanwa_data_update_can_hx_rocket_status(can_hx_rocket_status_t *data);

void tanwa_data_update_can_hx_rocket_data(can_hx_rocket_data_t *data);

void tanwa_data_update_can_hx_oxidizer_status(can_hx_oxidizer_status_t *data);

void tanwa_data_update_can_hx_oxidizer_data(can_hx_oxidizer_data_t *data);

void tanwa_data_update_can_fac_status(can_fac_status_t *data);

void tanwa_data_update_can_flc_status(can_flc_status_t *data);

void tanwa_data_update_can_flc_data(can_flc_data_t *data);

void tanwa_data_update_can_termo_status(can_termo_status_t *data);

void tanwa_data_update_can_termo_data(can_termo_data_t *data);

///===-----------------------------------------------------------------------------------------===//
/// read functions
///===-----------------------------------------------------------------------------------------===//

tanwa_data_t tanwa_data_read(void);

com_data_t tanwa_data_read_com_data(void);

can_connected_slaves_t tanwa_data_read_can_connected_slaves(void);

can_hx_rocket_status_t tanwa_data_read_can_hx_rocket_status(void);

can_hx_rocket_data_t tanwa_data_read_can_hx_rocket_data(void);

can_hx_oxidizer_status_t tanwa_data_read_can_hx_oxidizer_status(void);

can_hx_oxidizer_data_t tanwa_data_read_can_hx_oxidizer_data(void);

can_fac_status_t tanwa_data_read_can_fac_status(void);

can_flc_status_t tanwa_data_read_can_flc_status(void);

can_flc_data_t tanwa_data_read_can_flc_data(void);

can_termo_status_t tanwa_data_read_can_termo_status(void);

can_termo_data_t tanwa_data_read_can_termo_data(void);

#endif // PWRINSPACE_TANWA_DATA_H_