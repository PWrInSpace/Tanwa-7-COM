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
#include "mcu_adc_config.h"
#include "mcu_misc_config.h"

#include "led_driver.h"
#include "tmp1075.h"
#include "mcp23018.h"
#include "ads1115.h"
#include "pca9574.h"

#include "led_state_display.h"
#include "pressure_driver.h"
#include "solenoid_driver.h"

#define IOEXP_MODE  (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)

led_struct_t esp_led = {
    ._gpio_set_level = _GPIO_set_level,
    ._delay = _delay_ms,
    .gpio_num = CONFIG_GPIO_LED,
    .state = LED_STATE_OFF,
};

tmp1075_struct_t tmp1075_1 = {
    ._i2c_write = mcu_i2c_write,
    ._i2c_read = mcu_i2c_read,
    .i2c_address = CONFIG_I2C_TMP1075_TS1_ADDR,
    .config_register = 0,
};

tmp1075_struct_t tmp1075_2 = {
    ._i2c_write = mcu_i2c_write,
    ._i2c_read = mcu_i2c_read,
    .i2c_address = CONFIG_I2C_TMP1075_TS2_ADDR,
    .config_register = 0,
};

mcp23018_struct_t mcp23018 = {
    ._i2c_write = mcu_i2c_write,
    ._i2c_read = mcu_i2c_read,
    .i2c_address = CONFIG_I2C_MCP23018_ADDR,
    .iocon = 0,
    .dirRegisters = {0, 0},
    .polRegisters = {0, 0},
    .pullupRegisters = {0, 0},
    .ports = {0, 0},
};

ads1115_struct_t ads1115 = {
    ._i2c_write = mcu_i2c_write,
    ._i2c_read = mcu_i2c_read,
    .i2c_address = CONFIG_I2C_ADS1115_ADDR,
};

pca9574_struct_t pca9574 = {
    ._i2c_write = mcu_i2c_write,
    ._i2c_read = mcu_i2c_read,
    .i2c_address = CONFIG_I2C_PCA9574_ADDR,
};

led_state_display_struct_t led_state_display = {
    .mcp23018 = &mcp23018,
    .state = LED_STATE_DISPLAY_STATE_NONE,
};

pressure_driver_struct_t pressure_driver = PRESSURE_DRIVER_DEFAULT_CONFIG();

solenoid_driver_struct_t solenoid_driver = SOLENOID_DRIVER_DEFAULT_CONFIG();

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

    mcu_i2c_init();
    mcu_adc_init();
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // voltage_measure_init(&voltage_measure_config);
    // vTaskDelay(50 / portTICK_PERIOD_MS);

    led_GPIO_init(&esp_led);

    tmp1075_init(&tmp1075_1);
    tmp1075_init(&tmp1075_2);
    mcp23018_init(&mcp23018, IOEXP_MODE);
    pca9574_init(&pca9574);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    pressure_driver.ads1115 = &ads1115;
    pressure_driver_init(&pressure_driver);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    solenoid_driver.pca9574 = &pca9574;
    solenoid_driver_init(&solenoid_driver);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    // set all pins to low
    mcp23018_digital_write_port(&mcp23018, MCP23018_PORT_A, MCP23018_ALL_HIGH);
    mcp23018_digital_write_port(&mcp23018, MCP23018_PORT_B, MCP23018_ALL_HIGH);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    led_state_display_state_update(&led_state_display, LED_STATE_DISPLAY_STATE_IDLE);

    while (1) {
        int16_t raw;
        float temp, voltage, pressure;
        tmp1075_get_temp_raw(&tmp1075_1, &raw);
        tmp1075_get_temp_celsius(&tmp1075_1, &temp);
        printf("#TS1=> raw: %d, temp: %f\n", raw, temp);
        tmp1075_get_temp_raw(&tmp1075_2, &raw);
        tmp1075_get_temp_celsius(&tmp1075_2, &temp);
        printf("#TS2=> raw: %d, temp: %f\n", raw, temp);
        led_toggle(&esp_led);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        pressure_driver_read_voltage(&pressure_driver, PRESSURE_DRIVER_SENSOR_2, &voltage);
        pressure_driver_read_pressure(&pressure_driver, PRESSURE_DRIVER_SENSOR_2, &pressure);
        printf("#ADC=> voltage: %f, pressure: %f\n", voltage, pressure);
        mcu_adc_read_voltage(VBAT_CHANNEL, &voltage);
        printf("#VBAT=> voltage: %f\n", voltage);
        if (led_state_display.state == LED_STATE_DISPLAY_STATE_IDLE) {
            led_state_display_state_update(&led_state_display, LED_STATE_DISPLAY_STATE_ARMED);
        } else {
            led_state_display_state_update(&led_state_display, LED_STATE_DISPLAY_STATE_IDLE);
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        solenoid_driver_valve_toggle(&solenoid_driver, SOLENOID_DRIVER_VALVE_FILL);
        solenoid_driver_valve_toggle(&solenoid_driver, SOLENOID_DRIVER_VALVE_DEPR);
        solenoid_driver_valve_toggle(&solenoid_driver, SOLENOID_DRIVER_VALVE_ADD);
        led_toggle(&esp_led);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
