///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 24.07.2024 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//

#include "LIQUID_data.h"

#include <memory.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#define TAG "LIQUID_DATA"

static liquid_data_t liquid_data;
SemaphoreHandle_t liquid_data_mutex;

bool liquid_data_init(void) {
    liquid_data_mutex = NULL;
    liquid_data_mutex = xSemaphoreCreateMutex();
    if (liquid_data_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return false;
    }

    memset(&liquid_data, 0, sizeof(liquid_data_t));
    return true;
}

///===-----------------------------------------------------------------------------------------===//
/// update functions
///===-----------------------------------------------------------------------------------------===//

void liquid_data_update_state(uint8_t state) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        liquid_data.state = state;
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update LIQUID | Failed mutex");
    }
}

void liquid_data_update_main_valve_pressure_data(now_main_valve_pressure_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.pressure_data, data, sizeof(now_main_valve_pressure_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update MAIN VALVE PRESSURE | Failed mutex");
    }
}

void liquid_data_update_main_valve_temperature_data(now_main_valve_temperature_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.temperature_data, data, sizeof(now_main_valve_temperature_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update MAIN VALVE TEMPERATURE | Failed mutex");
    }
}

void liquid_data_update_can_flc_data(can_flc_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.can_flc_data, data, sizeof(can_flc_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update FLC | Failed mutex");
    }
}

void liquid_data_update_can_hx_oxidizer_data(can_hx_oxidizer_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.can_hx_oxidizer_data, data, sizeof(can_hx_oxidizer_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX OXIDIZER | Failed mutex");
    }
}

void liquid_data_update_can_hx_rocket_data(can_hx_rocket_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.can_hx_rocket_data, data, sizeof(can_hx_rocket_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX ROCKET | Failed mutex");
    }
}

void liquid_data_update_can_hx_rocket_data(can_hx_rocket_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.can_hx_rocket_data, data, sizeof(can_hx_rocket_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update HX ROCKET | Failed mutex");
    }
}

void liquid_data_update_now_termo_data(can_termo_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.now_termo_data, data, sizeof(can_termo_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update TERMO | Failed mutex");
    }
}

void liquid_data_update_can_fac_status(can_fac_status_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.can_fac_status, data, sizeof(can_fac_status_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update FAC | Failed mutex");
    }
}

void liquid_data_update_com_liquid_data(com_liquid_data_t *data) {
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&liquid_data.com_liquid_data, data, sizeof(com_liquid_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Update COM | Failed mutex");
    }
}

///===-----------------------------------------------------------------------------------------===//
/// read functions
///===-----------------------------------------------------------------------------------------===//

liquid_data_t liquid_data_read(void) {
    liquid_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data, sizeof(liquid_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read LIQUID | Failed mutex");
    }
    return data;
}

now_main_valve_pressure_data_t liquid_data_read_pressure_data(void) {
    now_main_valve_pressure_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.pressure_data, sizeof(now_main_valve_pressure_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read MAIN VALVE PRESSURE | Failed mutex");
    }
    return data;
}

now_main_valve_temperature_data_t liquid_data_read_temperature_data(void) {
    now_main_valve_temperature_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.temperature_data, sizeof(now_main_valve_temperature_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read MAIN VALVE TEMPERATURE | Failed mutex");
    }
    return data;
}

can_flc_data_t liquid_data_read_can_flc_data(void) {
    can_flc_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.can_flc_data, sizeof(can_flc_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read FLC | Failed mutex");
    }
    return data;
}

can_hx_oxidizer_data_t liquid_data_read_can_hx_oxidizer_data(void) {
    can_hx_oxidizer_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.can_hx_oxidizer_data, sizeof(can_hx_oxidizer_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read HX OXIDIZER | Failed mutex");
    }
    return data;
}

can_hx_rocket_data_t liquid_data_read_can_hx_rocket_data(void) {
    can_hx_rocket_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.can_hx_rocket_data, sizeof(can_hx_rocket_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read HX ROCKET | Failed mutex");
    }
    return data;
}

can_termo_data_t liquid_data_read_now_termo_data(void) {
    can_termo_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.now_termo_data, sizeof(can_termo_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read TERMO | Failed mutex");
    }
    return data;
}

can_fac_status_t liquid_data_read_can_fac_status(void) {
    can_fac_status_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.can_fac_status, sizeof(can_fac_status_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read FAC | Failed mutex");
    }
    return data;
}

com_liquid_data_t liquid_data_read_com_liquid_data(void) {
    com_liquid_data_t data;
    if (xSemaphoreTake(liquid_data_mutex, 1000) == pdTRUE) {
        memcpy(&data, &liquid_data.com_liquid_data, sizeof(com_liquid_data_t));
        xSemaphoreGive(liquid_data_mutex);
    } else {
        ESP_LOGE(TAG, "Read COM | Failed mutex");
    }
    return data;
}

