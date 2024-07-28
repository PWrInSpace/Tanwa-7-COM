///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 30.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "TANWA_data.h"

#include <memory.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#define TAG "TANWA_DATA"

static tanwa_data_t tanwa_data;
SemaphoreHandle_t tanwa_data_mutex;

bool tanwa_data_init(void) {
    tanwa_data_mutex = NULL;
    tanwa_data_mutex = xSemaphoreCreateMutex();
    if (tanwa_data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return false;
    }

    memset(&tanwa_data, 0, sizeof(tanwa_data_t));
    return true;
}

///===-----------------------------------------------------------------------------------------===//
/// update functions
///===-----------------------------------------------------------------------------------------===//

void tanwa_data_update_state(uint8_t state) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        tanwa_data.state = state;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update TANWA | Failed mutex");
    }
}

void tanwa_data_update_com_data(com_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.com_data, data, sizeof(com_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update COM | Failed mutex");
    }
}

void tanwa_data_update_can_connected_slaves(can_connected_slaves_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_connected_slaves, data, sizeof(can_connected_slaves_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update connected slaves | Failed mutex");
    }
}

void tanwa_data_update_can_hx_rocket_status(can_hx_rocket_status_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_hx_rocket_status, data, sizeof(can_hx_rocket_status_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX Rocket status | Failed mutex");
    }
}

void tanwa_data_update_can_hx_rocket_data(can_hx_rocket_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_hx_rocket_data, data, sizeof(can_hx_rocket_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX Rocket data | Failed mutex");
    }
}

void tanwa_data_update_can_hx_oxidizer_status(can_hx_oxidizer_status_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_hx_oxidizer_status, data, sizeof(can_hx_oxidizer_status_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX Oxidizer status | Failed mutex");
    }
}

void tanwa_data_update_can_hx_oxidizer_data(can_hx_oxidizer_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_hx_oxidizer_data, data, sizeof(can_hx_oxidizer_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX Oxidizer data | Failed mutex");
    }
}

void tanwa_data_update_can_fac_status(can_fac_status_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_fac_status, data, sizeof(can_fac_status_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update FAC status | Failed mutex");
    }
}

void tanwa_data_update_can_flc_status(can_flc_status_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_flc_status, data, sizeof(can_flc_status_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update FLC status | Failed mutex");
    }
}

void tanwa_data_update_can_flc_data(can_flc_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_flc_data, data, sizeof(can_flc_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update FLC data | Failed mutex");
    }
}

void tanwa_data_update_can_termo_status(can_termo_status_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_termo_status, data, sizeof(can_termo_status_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update TERMO status | Failed mutex");
    }
}

void tanwa_data_update_can_termo_data(can_termo_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.can_termo_data, data, sizeof(can_termo_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update TERMO data | Failed mutex");
    }
}

void tanwa_data_update_now_main_valve_pressure_data(now_main_valve_pressure_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.now_main_valve_pressure_data, data, sizeof(now_main_valve_pressure_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update Main Valve Pressure data | Failed mutex");
    }
}

void tanwa_data_update_now_main_valve_temperature_data(now_main_valve_temperature_data_t *data) {
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        memcpy(&tanwa_data.now_main_valve_temperature_data, data, sizeof(now_main_valve_temperature_data_t));
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update Main Valve Temperature data | Failed mutex");
    }
}

///===-----------------------------------------------------------------------------------------===//
/// read functions
///===-----------------------------------------------------------------------------------------===//

tanwa_data_t tanwa_data_read(void) {
    tanwa_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read TANWA | Failed mutex");
    }
    return data;
}

com_data_t tanwa_data_read_com_data(void) {
    com_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.com_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read COM | Failed mutex");
    }
    return data;
}

can_connected_slaves_t tanwa_data_read_can_connected_slaves(void) {
    can_connected_slaves_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_connected_slaves;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read connected slaves | Failed mutex");
    }
    return data;
}

can_hx_rocket_status_t tanwa_data_read_can_hx_rocket_status(void) {
    can_hx_rocket_status_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_hx_rocket_status;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read HX Rocket status | Failed mutex");
    }
    return data;
}

can_hx_rocket_data_t tanwa_data_read_can_hx_rocket_data(void) {
    can_hx_rocket_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_hx_rocket_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read HX Rocket data | Failed mutex");
    }
    return data;
}

can_hx_oxidizer_status_t tanwa_data_read_can_hx_oxidizer_status(void) {
    can_hx_oxidizer_status_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_hx_oxidizer_status;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read HX Oxidizer status | Failed mutex");
    }
    return data;
}

can_hx_oxidizer_data_t tanwa_data_read_can_hx_oxidizer_data(void) {
    can_hx_oxidizer_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_hx_oxidizer_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read HX Oxidizer data | Failed mutex");
    }
    return data;
}

can_fac_status_t tanwa_data_read_can_fac_status(void) {
    can_fac_status_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_fac_status;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read FAC status | Failed mutex");
    }
    return data;
}

can_flc_status_t tanwa_data_read_can_flc_status(void) {
    can_flc_status_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_flc_status;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read FLC status | Failed mutex");
    }
    return data;
}

can_flc_data_t tanwa_data_read_can_flc_data(void) {
    can_flc_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_flc_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read FLC data | Failed mutex");
    }
    return data;
}

can_termo_status_t tanwa_data_read_can_termo_status(void) {
    can_termo_status_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_termo_status;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read TERMO status | Failed mutex");
    }
    return data;
}

can_termo_data_t tanwa_data_read_can_termo_data(void) {
    can_termo_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.can_termo_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read TERMO data | Failed mutex");
    }
    return data;
}

now_main_valve_pressure_data_t tanwa_data_read_now_main_valve_pressure_data(void) {
    now_main_valve_pressure_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.now_main_valve_pressure_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read Main Valve Pressure data | Failed mutex");
    }
    return data;
}

now_main_valve_temperature_data_t tanwa_data_read_now_main_valve_temperature_data(void) {
    now_main_valve_temperature_data_t data = {0};
    if (xSemaphoreTake(tanwa_data_mutex, 1000) == pdTRUE) {
        data = tanwa_data.now_main_valve_temperature_data;
        xSemaphoreGive(tanwa_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read Main Valve Temperature data | Failed mutex");
    }
    return data;
}