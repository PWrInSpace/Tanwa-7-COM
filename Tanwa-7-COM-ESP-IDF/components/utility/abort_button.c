///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 19.03.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "abort_button.h"

#include "esp_log.h"

// #include "mcu_gpio_config.h"

// #define TAG "ABORT_BUTTON"

// static void IRAM_ATTR gpio_isr_handler(void* arg) {
//     abort_button_struct_t *abort_button = (abort_button_struct_t*) arg;
//     abort_button->isr_counter++;
//     if (abort_button->isr_counter > 10) {
//         abort_button->isr_counter = 0;
//         abort_button->callback();
//     }
// }

// abort_button_status_t abort_button_init(abort_button_struct_t *abort_button, void (*isr_handler)()) {
//     if (abort_button == NULL || isr_handler == NULL) {
//         return ABORT_BUTTON_ERROR;
//     }
//     esp_err_t res = ESP_OK;

//     res = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
//     if (res != ESP_OK) {
//         return ABORT_BUTTON_ERROR;
//     }

//     res = gpio_isr_handler_add(abort_button->pin, gpio_isr_handler, (void* arg) abort_button);
//     if (res != ESP_OK) {
//         return ABORT_BUTTON_ERROR;
//     }

//     return ABORT_BUTTON_OK;
// }