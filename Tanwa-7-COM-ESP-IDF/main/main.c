/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#include "mcu_i2c_config.h"
#include "mcu_misc_config.h"

#include "led_driver.h"
#include "tmp1075.h"
#include "mcp23018.h"
#include "ads1115.h"
#include "pca9574.h"

#define IOEXP_MODE  (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)

mcu_i2c_config_t i2c_config = MCU_I2C_DEFAULT_CONFIG();

led_struct_t esp_led = {
    ._gpio_set_level = _GPIO_set_level,
    ._delay = _delay_ms,
    .gpio_num = CONFIG_GPIO_LED,
    .state = LED_STATE_OFF,
};

tmp1075_struct_t tmp1075_1 = {
    ._i2c_write = _I2C_write,
    ._i2c_read = _I2C_read,
    .i2c_address = CONFIG_I2C_TMP1075_TS1_ADDR,
    .config_register = 0,
};

tmp1075_struct_t tmp1075_2 = {
    ._i2c_write = _I2C_write,
    ._i2c_read = _I2C_read,
    .i2c_address = CONFIG_I2C_TMP1075_TS2_ADDR,
    .config_register = 0,
};

mcp23018_struct_t mcp23018 = {
    ._i2c_write = _I2C_write,
    ._i2c_read = _I2C_read,
    .i2c_address = CONFIG_I2C_MCP23018_ADDR,
    .iocon = 0,
    .dirRegisters = {0, 0},
    .polRegisters = {0, 0},
    .pullupRegisters = {0, 0},
    .ports = {0, 0},
};

ads1115_struct_t ads1115 = {
    ._i2c_write = _I2C_write,
    ._i2c_read = _I2C_read,
    .i2c_address = CONFIG_I2C_ADS1115_ADDR,
};

pca9574_struct_t pca9574 = {
    ._i2c_write = _I2C_write,
    ._i2c_read = _I2C_read,
    .i2c_address = CONFIG_I2C_PCA9574_ADDR,
};

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    // for (int i = 10; i >= 0; i--) {
    //     printf("Restarting in %d seconds...\n", i);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();

    i2c_init(&i2c_config);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    led_GPIO_init(&esp_led);

    tmp1075_init(&tmp1075_1);
    tmp1075_init(&tmp1075_2);
    mcp23018_init(&mcp23018, IOEXP_MODE);
    pca9574_init(&pca9574);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // set all pins to output
    mcp23018_set_port_mode(&mcp23018, PORT_A, ALL_OUTPUT);
    mcp23018_set_port_mode(&mcp23018, PORT_B, ALL_OUTPUT);

    // set all pins to low
    mcp23018_digital_write_port(&mcp23018, PORT_A, ALL_LOW);
    mcp23018_digital_write_port(&mcp23018, PORT_B, ALL_LOW);

    ads1115_set_mode(&ads1115, ADS1115_MODE_CONTINUOUS);
    ads1115_set_data_rate(&ads1115, ADS1115_DATA_RATE_32);
    ads1115_set_input_mux(&ads1115, ADS1115_MUX_1_GND);
    ads1115_set_gain(&ads1115, ADS1115_GAIN_4V096);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    pca9574_set_mode(&pca9574, PCA9574_OUTPUT);
    pca9574_set_level(&pca9574, PCA9574_HIGH);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    while (1) {
        int16_t raw;
        float temp;
        tmp1075_get_temp_raw(&tmp1075_1, &raw);
        tmp1075_get_temp_celsius(&tmp1075_1, &temp);
        printf("#TS1=> raw: %d, temp: %f\n", raw, temp);
        tmp1075_get_temp_raw(&tmp1075_2, &raw);
        tmp1075_get_temp_celsius(&tmp1075_2, &temp);
        printf("#TS2=> raw: %d, temp: %f\n", raw, temp);
        led_toggle(&esp_led);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ads1115_get_value(&ads1115, &raw);
        float voltage = ads1115_gain_values[ADS1115_GAIN_4V096] / ADS1115_MAX_VALUE * raw;
        printf("#ADC=> raw: %d, voltage: %f\n", raw, voltage);
        led_toggle(&esp_led);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        mcp23018_digital_write_port(&mcp23018, PORT_A, ALL_HIGH);
        mcp23018_digital_write_port(&mcp23018, PORT_B, ALL_HIGH);
        led_toggle(&esp_led);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        mcp23018_digital_write_port(&mcp23018, PORT_A, ALL_LOW);
        mcp23018_digital_write_port(&mcp23018, PORT_B, ALL_LOW);
        led_toggle(&esp_led);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
