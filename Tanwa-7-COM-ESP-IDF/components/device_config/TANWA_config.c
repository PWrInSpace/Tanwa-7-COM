///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "TANWA_config.h"

#include "esp_log.h"

#include "mcu_gpio_config.h"
#include "mcu_adc_config.h"
#include "mcu_i2c_config.h"
#include "mcu_misc_config.h"

#define TAG "TANWA_CONFIG"

#define IOEXP_MODE (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)

TANWA_hardware_t TANWA_hardware = {
    .esp_led = {
        ._gpio_set_level = mcu_gpio_set_level,
        ._delay = _delay_ms,
        .gpio_num = LED_GPIO_INDEX,
        .state = LED_STATE_OFF,
    },
    .tmp1075 = {
        {
            ._i2c_write = mcu_i2c_write,
            ._i2c_read = mcu_i2c_read,
            .i2c_address = CONFIG_I2C_TMP1075_TS1_ADDR,
            .config_register = 0,
        },
        {
            ._i2c_write = mcu_i2c_write,
            ._i2c_read = mcu_i2c_read,
            .i2c_address = CONFIG_I2C_TMP1075_TS2_ADDR,
            .config_register = 0,
        },
    },
    .mcp23018 = {
        ._i2c_write = mcu_i2c_write,
        ._i2c_read = mcu_i2c_read,
        .i2c_address = CONFIG_I2C_MCP23018_ADDR,
        .iocon = 0,
        .dirRegisters = {0, 0},
        .polRegisters = {0, 0},
        .pullupRegisters = {0, 0},
        .ports = {0, 0},
    },
    .ads1115 = {
        ._i2c_write = mcu_i2c_write,
        ._i2c_read = mcu_i2c_read,
        .i2c_address = CONFIG_I2C_ADS1115_ADDR,
    },
    .pca9574 = {
        ._i2c_write = mcu_i2c_write,
        ._i2c_read = mcu_i2c_read,
        .i2c_address = CONFIG_I2C_PCA9574_ADDR,
    },
    .igniter = {
        {
            ._adc_analog_read_raw = mcu_adc_read_raw,
            ._gpio_set_level = mcu_gpio_set_level,
            ._delay = _delay_ms,
            .adc_channel_continuity = IGNITER_1_CHANNEL_INDEX,
            .gpio_num_arm = ARM_GPIO_INDEX,
            .gpio_num_fire = FIRE_1_GPIO_INDEX,
            .drive = IGNITER_DRIVE_POSITIVE,
            .state = IGNITER_STATE_WAITING,
        },
        {
            ._adc_analog_read_raw = mcu_adc_read_raw,
            ._gpio_set_level = mcu_gpio_set_level,
            ._delay = _delay_ms,
            .adc_channel_continuity = IGNITER_2_CHANNEL_INDEX,
            .gpio_num_arm = ARM_GPIO_INDEX,
            .gpio_num_fire = FIRE_2_GPIO_INDEX,
            .drive = IGNITER_DRIVE_POSITIVE,
            .state = IGNITER_STATE_WAITING,
        },
    },
};

TANWA_utility_t TANWA_utility = {
    .led_state_display = {
        .mcp23018 = &TANWA_hardware.mcp23018,
        .state = LED_STATE_DISPLAY_STATE_NONE,
    },
    .pressure_driver = PRESSURE_DRIVER_TANWA_CONFIG(&TANWA_hardware.ads1115),
    .solenoid_driver = SOLENOID_DRIVER_TANWA_CONFIG(&TANWA_hardware.pca9574),
};

esp_err_t TANWA_mcu_config_init() {
    esp_err_t ret = ESP_OK;
    ret |= mcu_gpio_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize GPIO");
        return ret;
    }
    ret |= mcu_adc_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC");
        return ret;
    }
    ret |= mcu_i2c_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C");
        return ret;
    }
    return ESP_OK;
}

esp_err_t TANWA_hardware_init() {
    uint8_t ret = 0;
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[0]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor 1");
        return ESP_FAIL;
    }
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[1]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor 2");
        return ESP_FAIL;
    }
    ret = mcp23018_init(&(TANWA_hardware.mcp23018), IOEXP_MODE);
    if (ret != MCP23018_OK) {
        ESP_LOGE(TAG, "Failed to initialize MCP23018");
        return ESP_FAIL;
    }
    ret = pca9574_init(&(TANWA_hardware.pca9574));
    if (ret != true) {
        ESP_LOGE(TAG, "Failed to initialize PCA9574");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t TANWA_utility_init() {
    uint8_t ret = 0;
    ret = led_state_display_state_update(&(TANWA_utility.led_state_display), LED_STATE_DISPLAY_STATE_IDLE);
    if (ret != LED_STATE_DISPLAY_OK) {
        ESP_LOGE(TAG, "Failed to initialize LED state display");
        return ESP_FAIL;
    }
    ret = pressure_driver_init(&(TANWA_utility.pressure_driver));
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Failed to initialize pressure driver");
        return ESP_FAIL;
    }
    ret = solenoid_driver_init(&(TANWA_utility.solenoid_driver));
    if (ret != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "Failed to initialize solenoid driver");
        return ESP_FAIL;
    }
    return ESP_OK;
}

// esp_err_t TANWA_get_hardware(TANWA_hardware_t** hardware) {
//     *hardware = &TANWA_hardware;
//     return ESP_OK;
// }

// esp_err_t TANWA_get_utility(TANWA_utility_t** utility) {
//     *utility = &TANWA_utility;
//     return ESP_OK;
// }
