///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 17.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#include "mcu_gpio_config.h"

#include "esp_log.h"

#define TAG "MCU_GPIO_CONFIG"

static mcu_gpio_config_t mcu_gpio_config = {
    .pins = {LED_GPIO, ARM_GPIO, FIRE_1_GPIO, FIRE_2_GPIO},
    .num_pins = MAX_GPIO_INDEX,
    .configs = {
        {
            .pin_bit_mask = (1ULL << LED_GPIO),
            .mode = GPIO_MODE_OUTPUT_OD,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        },
        {
            .pin_bit_mask = (1ULL << ABORT_GPIO),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_NEGEDGE,
        },
        {
            .pin_bit_mask = (1ULL << ARM_GPIO),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        },
        {
            .pin_bit_mask = (1ULL << FIRE_1_GPIO),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        },
        {
            .pin_bit_mask = (1ULL << FIRE_2_GPIO),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        }
    },
};

esp_err_t mcu_gpio_init() {
    esp_err_t res = ESP_OK;

    for (uint8_t i = 0; i < mcu_gpio_config.num_pins; i++) {
        res |= gpio_config(&mcu_gpio_config.configs[i]);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "GPIO pin %d configuration failed!", i);
        }
    }

    for (uint8_t i = 0; i < mcu_gpio_config.num_pins; i++) {
        res |= _mcu_gpio_set_level(mcu_gpio_config.pins[i], 0);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "GPIO pin %d level set failed!", i);
        }
    }

    return res;
}

bool _mcu_gpio_set_level(uint8_t gpio, uint8_t level) {
    return gpio_set_level(mcu_gpio_config.pins[gpio], level) == ESP_OK ? true : false;
}

bool _mcu_gpio_get_level(uint8_t gpio, uint8_t* level) {
    uint8_t res = gpio_get_level(mcu_gpio_config.pins[gpio]);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "GPIO pin %d read failed!", mcu_gpio_config.pins[gpio]);
        return false;
    }
    *level = (uint8_t)res;
    return true;
}