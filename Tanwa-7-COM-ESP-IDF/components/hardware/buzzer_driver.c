// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "buzzer_driver.h"

#include "esp_log.h"

#define TAG "BUZZER_DRIVER"

buzzer_driver_status_t buzzer_init(buzzer_struct_t *config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Invalid argument - NULL check failed");
        return BUZZER_NULL_ARG;
    }
    if (config->polarity == BUZZER_POLARITY_ACTIVE_HIGH) {
        if (!config->_gpio_set_level(config->gpio_idx, 0)) {
            ESP_LOGE(TAG, "Failed to set buzzer gpio");
            return BUZZER_GPIO_ERR;
        }
    } else if (config->polarity == BUZZER_POLARITY_ACTIVE_LOW) {
        if (!config->_gpio_set_level(config->gpio_idx, 1)) {
            ESP_LOGE(TAG, "Failed to set buzzer gpio");
            return BUZZER_GPIO_ERR;
        }
    } else {
        ESP_LOGE(TAG, "Invalid argument - out of value range");
        return BUZZER_INVALID_ARG;
    }
    return BUZZER_OK;
}

buzzer_driver_status_t buzzer_set_state(buzzer_struct_t *config, buzzer_driver_state_t state) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Invalid argument - NULL check failed");
        return BUZZER_NULL_ARG;
    }
    if (state == BUZZER_STATE_ON) {
        if (config->polarity == BUZZER_POLARITY_ACTIVE_HIGH) {
            if (!config->_gpio_set_level(config->gpio_idx, 1)) {
                ESP_LOGE(TAG, "Failed to turn ON");
                return BUZZER_GPIO_ERR;
            }
        } else if (config->polarity == BUZZER_POLARITY_ACTIVE_LOW) {
            if (!config->_gpio_set_level(config->gpio_idx, 0)) {
                ESP_LOGE(TAG, "Failed to turn ON");
                return BUZZER_GPIO_ERR;
            }
        } else {
            ESP_LOGE(TAG, "Invalid argument - out of value range");
            return BUZZER_INVALID_ARG;
        }
        config->state = BUZZER_STATE_ON;
        return BUZZER_OK;
    } else if (state == BUZZER_STATE_OFF) {
        if (config->polarity == BUZZER_POLARITY_ACTIVE_HIGH) {
            if (!config->_gpio_set_level(config->gpio_idx, 0)) {
                ESP_LOGE(TAG, "Failed to turn OFF");
                return BUZZER_GPIO_ERR;
            }
        } else if (config->polarity == BUZZER_POLARITY_ACTIVE_LOW) {
            if (!config->_gpio_set_level(config->gpio_idx, 1)) {
                ESP_LOGE(TAG, "Failed to turn OFF");
                return BUZZER_GPIO_ERR;
            }
        } else {
            ESP_LOGE(TAG, "Invalid argument - out of value range");
            return BUZZER_INVALID_ARG;
        }
        config->state = BUZZER_STATE_OFF;
        return BUZZER_OK;
    } else {
        ESP_LOGE(TAG, "Invalid argument - out of value range");
        return BUZZER_INVALID_ARG;
    }
}

buzzer_driver_status_t buzzer_get_state(buzzer_struct_t *config, buzzer_driver_state_t *state) {
    if (config == NULL || state == NULL) {
        ESP_LOGE(TAG, "Invalid argument - NULL check failed");
        return BUZZER_NULL_ARG;
    }
    *state = config->state;
    return BUZZER_OK;
}

buzzer_driver_status_t buzzer_toggle_state(buzzer_struct_t *config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Invalid argument - NULL check failed");
        return BUZZER_NULL_ARG;
    }
    if (config->state == BUZZER_STATE_ON) {
        return buzzer_set_state(config, BUZZER_STATE_OFF);
    } else if (config->state == BUZZER_STATE_OFF) {
        return buzzer_set_state(config, BUZZER_STATE_ON);
    } else {
        ESP_LOGE(TAG, "Invalid argument - out of value range");
        return BUZZER_INVALID_ARG;
    }
}