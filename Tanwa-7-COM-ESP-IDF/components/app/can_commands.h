///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 31.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the CAN BUS commands that are used in the communication over
/// the CAN BUS with TANWA submodules.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_CAN_COMMANDS_H_
#define PWRINSPACE_TANWA_CAN_COMMANDS_H_

#include "mcu_twai_config.h"

///===-----------------------------------------------------------------------------------------===//
// CAN commands IDs
///===-----------------------------------------------------------------------------------------===//

typedef enum {
    // Commands to the HX submodule
    CAN_HX_RCK_TX_GET_STATUS = 0x0A0,
    CAN_HX_RCK_TX_GET_DATA = 0x0A1,
    CAN_HX_RCK_TX_TARE = 0x0A2,
    CAN_HX_RCK_TX_CALIBRATE = 0x0A3,
    CAN_HX_RCK_TX_SET_CALIBRATION_FACTOR = 0x0A4,
    CAN_HX_RCK_TX_SET_OFFSET = 0x0A5,
    // place for new commands
    CAN_HX_RCK_TX_SOFT_RESET = 0x0A9,
    CAN_HX_RCK_TX_NOTHING = 0x0AF,
    // Responses from the HX submodule
    CAN_HX_RCK_RX_STATUS = 0x0AA,
    CAN_HX_RCK_RX_DATA = 0x0AB,
} can_hx_rck_commands_t;

typedef enum {
    // Commands to the HX submodule
    CAN_HX_OXI_TX_GET_STATUS = 0x0B0,
    CAN_HX_OXI_TX_GET_DATA = 0x0B1,
    CAN_HX_OXI_TX_TARE = 0x0B2,
    CAN_HX_OXI_TX_CALIBRATE = 0x0B3,
    CAN_HX_OXI_TX_SET_CALIBRATION_FACTOR = 0x0B4,
    CAN_HX_OXI_TX_SET_OFFSET = 0x0B5,
    // place for new commands
    CAN_HX_OXI_TX_SOFT_RESET = 0x0B9,
    CAN_HX_OXI_TX_NOTHING = 0x0BF,
    // Responses from the HX submodule
    CAN_HX_OXI_RX_STATUS = 0x0BA,
    CAN_HX_OXI_RX_DATA = 0x0BB,
} can_hx_oxi_commands_t;

typedef enum {
    // Commands to the FAC submodule
    CAN_FAC_TX_GET_STATUS = 0x0C0,
    CAN_FAC_TX_QD_PULL = 0x0C1,
    CAN_FAC_TX_QD_STOP = 0x0C2,
    CAN_FAC_TX_QD_PUSH = 0x0C3,
    // place for new commands
    CAN_FAC_TX_SOFT_RESET = 0x0C9,
    CAN_FAC_TX_NOTHING = 0x0CF,
    // Responses from the FAC submodule
    CAN_FAC_RX_STATUS = 0x0CA,
} can_fac_commands_t;

typedef enum {
    // Commands to the FLC submodule
    CAN_FLC_TX_GET_STATUS = 0x0D0,
    CAN_FLC_TX_GET_DATA = 0x0D1,
    // place for new commands
    CAN_FLC_TX_SOFT_RESET = 0x0D9,
    CAN_FLC_TX_NOTHING = 0x0DF,
    // Responses from the FLC submodule
    CAN_FLC_RX_STATUS = 0x0DA,
    CAN_FLC_RX_DATA = 0x0DB,
} can_flc_commands_t;

typedef enum {
    // Commands to the TERMO submodule
    CAN_TERMO_TX_GET_STATUS = 0x0E0,
    CAN_TERMO_TX_GET_DATA = 0x0E1,
    CAN_TERMO_TX_HEAT_START = 0x0E2,
    CAN_TERMO_TX_HEAT_STOP = 0x0E3,
    CAN_TERMO_TX_COOL_START = 0x0E4,
    CAN_TERMO_TX_COOL_STOP = 0x0E5,
    CAN_TERMO_TX_SET_MAX_PRESSURE = 0x0E6,
    CAN_TERMO_TX_SET_MIN_PRESSURE = 0x0E7,
    // place for new commands
    CAN_TERMO_TX_SOFT_RESET = 0x0E9,
    CAN_TERMO_TX_NOTHING = 0x0EF,
    // Responses from the TERMO submodule
    CAN_TERMO_RX_STATUS = 0x0EA,
    CAN_TERMO_RX_DATA = 0x0EB,
} can_termo_commands_t;

///===-----------------------------------------------------------------------------------------===//
// CAN predefined messages
///===-----------------------------------------------------------------------------------------===//

// HX Rocket submodule

const twai_message_t can_hx_rck_tx_get_status = {
    .identifier = CAN_HX_RCK_TX_GET_STATUS,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_rck_tx_get_data = {
    .identifier = CAN_HX_RCK_TX_GET_DATA,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_rck_tx_tare = {
    .identifier = CAN_HX_RCK_TX_TARE,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_rck_tx_calibrate = {
    .identifier = CAN_HX_RCK_TX_CALIBRATE,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_rck_tx_set_calibration_factor = {
    .identifier = CAN_HX_RCK_TX_SET_CALIBRATION_FACTOR,
    .data_length_code = 4,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_rck_tx_set_offset = {
    .identifier = CAN_HX_RCK_TX_SET_OFFSET,
    .data_length_code = 4,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_rck_tx_soft_reset = {
    .identifier = CAN_HX_RCK_TX_SOFT_RESET,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

// HX Oxidizer submodule

const twai_message_t can_hx_oxi_tx_get_status = {
    .identifier = CAN_HX_OXI_TX_GET_STATUS,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_oxi_tx_get_data = {
    .identifier = CAN_HX_OXI_TX_GET_DATA,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_oxi_tx_tare = {
    .identifier = CAN_HX_OXI_TX_TARE,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_oxi_tx_calibrate = {
    .identifier = CAN_HX_OXI_TX_CALIBRATE,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_oxi_tx_set_calibration_factor = {
    .identifier = CAN_HX_OXI_TX_SET_CALIBRATION_FACTOR,
    .data_length_code = 4,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_oxi_tx_set_offset = {
    .identifier = CAN_HX_OXI_TX_SET_OFFSET,
    .data_length_code = 4,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_hx_oxi_tx_soft_reset = {
    .identifier = CAN_HX_OXI_TX_SOFT_RESET,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

// FAC submodule

const twai_message_t can_fac_tx_get_status = {
    .identifier = CAN_FAC_TX_GET_STATUS,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_fac_tx_qd_pull = {
    .identifier = CAN_FAC_TX_QD_PULL,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_fac_tx_qd_stop = {
    .identifier = CAN_FAC_TX_QD_STOP,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_fac_tx_qd_push = {
    .identifier = CAN_FAC_TX_QD_PUSH,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_fac_tx_soft_reset = {
    .identifier = CAN_FAC_TX_SOFT_RESET,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

// FLC submodule

const twai_message_t can_flc_tx_get_status = {
    .identifier = CAN_FLC_TX_GET_STATUS,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_flc_tx_get_data = {
    .identifier = CAN_FLC_TX_GET_DATA,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_flc_tx_soft_reset = {
    .identifier = CAN_FLC_TX_SOFT_RESET,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

// TERMO submodule

const twai_message_t can_termo_tx_get_status = {
    .identifier = CAN_TERMO_TX_GET_STATUS,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_get_data = {
    .identifier = CAN_TERMO_TX_GET_DATA,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_heat_start = {
    .identifier = CAN_TERMO_TX_HEAT_START,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_heat_stop = {
    .identifier = CAN_TERMO_TX_HEAT_STOP,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_cool_start = {
    .identifier = CAN_TERMO_TX_COOL_START,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_cool_stop = {
    .identifier = CAN_TERMO_TX_COOL_STOP,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_set_max_pressure = {
    .identifier = CAN_TERMO_TX_SET_MAX_PRESSURE,
    .data_length_code = 4,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_set_min_pressure = {
    .identifier = CAN_TERMO_TX_SET_MIN_PRESSURE,
    .data_length_code = 4,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

const twai_message_t can_termo_tx_soft_reset = {
    .identifier = CAN_TERMO_TX_SOFT_RESET,
    .data_length_code = 0,
    .data = {0, 0, 0, 0, 0, 0, 0, 0}
};

#endif /* PWRINSPACE_TANWA_CAN_COMMANDS_H_ */
