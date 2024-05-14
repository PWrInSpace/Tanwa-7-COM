///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 19.03.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the system ABORT button, which is connected to the GPIO pin
/// of the MCU (ESP32) and is used to trigger the emergency stop of the system.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_ABORT_BUTTON_H_
#define PWRINSPACE_ABORT_BUTTON_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define ABORT_BUTTON_TANWA_CONFIG(X)                \
    {                                               \
        .pin = X,                                   \
        .polarity = ABORT_BUTTTON_POLARITY_NEGATIVE,\
        .isr_counter = 0                            \
    }

typedef enum {
    ABORT_BUTTTON_POLARITY_POSITIVE = 0,
    ABORT_BUTTTON_POLARITY_NEGATIVE = 1
} abort_button_polarity_t;

typedef enum {
    ABORT_BUTTON_OK = 0,
    ABORT_BUTTON_ERROR = 1,
    ABORT_BUTTON_READ_ERROR = 2
} abort_button_status_t;

typedef struct {
    uint8_t pin;
    abort_button_polarity_t polarity;
    uint8_t isr_counter;
    void (*callback)();
} abort_button_struct_t;

abort_button_status_t abort_button_init(abort_button_struct_t *abort_button, void (*isr_handler)());

abort_button_status_t abort_button_check_state(abort_button_struct_t *abort_button);

#endif /* PWRINSPACE_ABORT_BUTTON_H_ */