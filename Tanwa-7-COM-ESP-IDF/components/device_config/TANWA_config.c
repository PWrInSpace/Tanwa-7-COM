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
#include "mcu_spi_config.h"
#include "mcu_twai_config.h"
#include "mcu_misc_config.h"

#define TAG "TANWA_CONFIG"

#define IOEXP_MODE (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)

TANWA_hardware_t TANWA_hardware = {
    .esp_led = {
        ._gpio_set_level = _mcu_gpio_set_level,
        ._delay = _LED_delay_ms,
        .gpio_num = LED_GPIO_INDEX,
        .state = LED_STATE_OFF,
    },
    .tmp1075 = {
        {
            ._i2c_write = _mcu_i2c_write,
            ._i2c_read = _mcu_i2c_read,
            .i2c_address = CONFIG_I2C_TMP1075_TS1_ADDR,
            .config_register = 0,
        },
        {
            ._i2c_write = _mcu_i2c_write,
            ._i2c_read = _mcu_i2c_read,
            .i2c_address = CONFIG_I2C_TMP1075_TS2_ADDR,
            .config_register = 0,
        },
    },
    .mcp23018 = {
        ._i2c_write = _mcu_i2c_write,
        ._i2c_read = _mcu_i2c_read,
        .i2c_address = CONFIG_I2C_MCP23018_ADDR,
        .iocon = 0,
        .dirRegisters = {0, 0},
        .polRegisters = {0, 0},
        .pullupRegisters = {0, 0},
        .ports = {0, 0},
    },
    .ads1115 = {
        ._i2c_write = _mcu_i2c_write,
        ._i2c_read = _mcu_i2c_read,
        .i2c_address = CONFIG_I2C_ADS1115_ADDR,
    },
    .pca9574 = {
        ._i2c_write = _mcu_i2c_write,
        ._i2c_read = _mcu_i2c_read,
        .i2c_address = CONFIG_I2C_PCA9574_ADDR,
    },
    .igniter = {
        {
            ._adc_analog_read_raw = _mcu_adc_read_raw,
            ._gpio_set_level = _mcu_gpio_set_level,
            ._delay = _IGNITER_delay_ms,
            .adc_channel_continuity = IGNITER_1_CHANNEL_INDEX,
            .gpio_num_arm = ARM_GPIO_INDEX,
            .gpio_num_fire = FIRE_1_GPIO_INDEX,
            .drive = IGNITER_DRIVE_POSITIVE,
            .state = IGNITER_STATE_WAITING,
        },
        {
            ._adc_analog_read_raw = _mcu_adc_read_raw,
            ._gpio_set_level = _mcu_gpio_set_level,
            ._delay = _IGNITER_delay_ms,
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

TANWA_lora_t TANWA_lora = {
    ._spi_transmit = _lora_SPI_transmit,
    ._delay = _lora_delay,
    ._gpio_set_level = _mcu_gpio_set_level,
    .log = _lora_log,
    .rst_gpio_num = LORA_RST_GPIO_INDEX,
    .cs_gpio_num = LORA_CS_GPIO_INDEX,
    .d0_gpio_num = LORA_D0_GPIO_INDEX,
    .implicit_header = 0, 
    .frequency = 0,
};

esp_err_t TANWA_mcu_config_init() {
    esp_err_t ret = ESP_OK;
    ret |= mcu_gpio_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize GPIO");
        return ret;
    } else {
        ESP_LOGI(TAG, "GPIO initialized");
    }
    ret |= mcu_adc_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC");
        return ret;
    } else {
        ESP_LOGI(TAG, "ADC initialized");
    }
    ret |= mcu_i2c_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C");
        return ret;
    } else {
        ESP_LOGI(TAG, "I2C initialized");
    }
    ret |= mcu_spi_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI");
        return ret;
    } else {
        ESP_LOGI(TAG, "SPI initialized");
    }
    ret |= mcu_twai_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize TWAI");
        return ret;
    } else {
        ESP_LOGI(TAG, "TWAI initialized");
    }
    return ESP_OK;
}

esp_err_t TANWA_hardware_init() {
    uint8_t ret = 0;
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[0]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor 1");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "TMP1075 sensor 1 initialized");
    }
    ret = tmp1075_init(&(TANWA_hardware.tmp1075[1]));
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "Failed to initialize TMP1075 sensor 2");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "TMP1075 sensor 2 initialized");
    }
    ret = mcp23018_init(&(TANWA_hardware.mcp23018), IOEXP_MODE);
    if (ret != MCP23018_OK) {
        ESP_LOGE(TAG, "Failed to initialize MCP23018");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "MCP23018 initialized");
    }
    ret = pca9574_init(&(TANWA_hardware.pca9574));
    if (ret != true) {
        ESP_LOGE(TAG, "Failed to initialize PCA9574");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "PCA9574 initialized");
    }
    return ESP_OK;
}

esp_err_t TANWA_utility_init() {
    uint8_t ret = 0;
    ret = led_state_display_state_update(&(TANWA_utility.led_state_display), LED_STATE_DISPLAY_STATE_IDLE);
    if (ret != LED_STATE_DISPLAY_OK) {
        ESP_LOGE(TAG, "Failed to initialize LED state display");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "LED state display initialized");
    }
    ret = pressure_driver_init(&(TANWA_utility.pressure_driver));
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Failed to initialize pressure driver");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Pressure driver initialized");
    }
    ret = solenoid_driver_init(&(TANWA_utility.solenoid_driver));
    if (ret != SOLENOID_DRIVER_OK) {
        ESP_LOGE(TAG, "Failed to initialize solenoid driver");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Solenoid driver initialized");
    }
    return ESP_OK;
}

esp_err_t TANWA_lora_init() {
    lora_err_t ret = 0;
    // LoRa init
    ret = lora_init(&TANWA_lora); 
    if (ret != LORA_OK) {
        ESP_LOGE(TAG, "Failed to initialize LoRa");
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "LoRa initialized");
    }
    // set lora config
    vTaskDelay(pdMS_TO_TICKS(100));
    ret = lora_set_frequency(&TANWA_lora, 867e6);
    if (ret != LORA_OK) {
        ESP_LOGE(TAG, "Failed to set LoRa frequency");
        return ESP_FAIL;
    }
    ret = lora_set_bandwidth(&TANWA_lora, LORA_BW_250_kHz); 
    if (ret != LORA_OK) {
        ESP_LOGE(TAG, "Failed to set LoRa bandwidth");
        return ESP_FAIL;
    }
    ret = lora_set_tx_power(&TANWA_lora, 17); 
    if (ret != LORA_OK) {
        ESP_LOGE(TAG, "Failed to set LoRa TX power");
        return ESP_FAIL;
    }
    ret = lora_set_spreading_factor(&TANWA_lora, 7);
    if (ret != LORA_OK) {
        ESP_LOGE(TAG, "Failed to set LoRa spreading factor");
        return ESP_FAIL;
    }
    ret = lora_disable_crc(&TANWA_lora);
    if (ret != LORA_OK) {
        ESP_LOGE(TAG, "Failed to disable LoRa CRC");
        return ESP_FAIL;
    }
    // self test lora
    const char* lora_packet = "TEST;";
    ESP_LOGI(TAG, "Reading LoRa registers");
    int16_t read_val_one = lora_read_reg(&TANWA_lora, 0x0d);
    int16_t read_val_two = lora_read_reg(&TANWA_lora, 0x0c);
    ESP_LOGI(TAG, "LORA_READ: %04x, %04x", read_val_one, read_val_two);
    lora_reset(&TANWA_lora);
    ESP_LOGI(TAG, "LoRa Reset");
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

void _lora_delay(size_t _ms) { vTaskDelay(pdMS_TO_TICKS(_ms)); }

void _lora_log(const char* info) { ESP_LOGI(TAG, "%s", info); }