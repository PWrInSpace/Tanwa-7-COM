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
#include "driver/i2c.h"
#include "tmp1075.h"
#include "mcp23018.h"

#define I2C_MASTER_SCL_IO           CONFIG_I2C_SCL              /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_SDA              /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define TS1_ADDR 0x4E
#define TS2_ADDR 0x4F
#define MCP23018_ADDR 0x21

#define IOEXP_MODE  (IOCON_INTCC | IOCON_INTPOL | IOCON_ODR | IOCON_MIRROR)

bool _tmp1075_I2C_write_TS1(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
bool _tmp1075_I2C_read_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
bool _tmp1075_I2C_write_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
bool _tmp1075_I2C_read_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
bool _mcp23018_I2C_write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);
bool _mcp23018_I2C_read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len);

bool _tmp1075_I2C_write_TS1(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    // dynamically create a buffer to hold the data to be written
    uint8_t *write_buf = malloc(len + 1);
    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, TS1_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    free(write_buf);
    return (bool)(ret == ESP_OK);
}

bool _tmp1075_I2C_read_TS1(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(I2C_MASTER_NUM, TS1_ADDR, &reg, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (bool)(ret == ESP_OK);
}

bool _tmp1075_I2C_write_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    // dynamically create a buffer to hold the data to be written
    uint8_t *write_buf = malloc(len + 1);
    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, TS2_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    free(write_buf);
    return (bool)(ret == ESP_OK);
}

bool _tmp1075_I2C_read_TS2(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(I2C_MASTER_NUM, TS2_ADDR, &reg, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (bool)(ret == ESP_OK);
}

bool _mcp23018_I2C_write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    // dynamically create a buffer to hold the data to be written
    uint8_t *write_buf = malloc(len + 1);
    write_buf[0] = reg;
    memcpy(write_buf + 1, data, len);
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MCP23018_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    free(write_buf);
    return (bool)(ret == ESP_OK);
}

bool _mcp23018_I2C_read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
    esp_err_t ret;
    ret = i2c_master_write_read_device(I2C_MASTER_NUM, MCP23018_ADDR, &reg, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return (bool)(ret == ESP_OK);
}

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

    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    tmp1075_struct_t tmp1075_1 = {
        ._i2c_write = _tmp1075_I2C_write_TS1,
        ._i2c_read = _tmp1075_I2C_read_TS1,
        .i2c_address = TS1_ADDR,
        .config_register = 0,
    };

    tmp1075_struct_t tmp1075_2 = {
        ._i2c_write = _tmp1075_I2C_write_TS2,
        ._i2c_read = _tmp1075_I2C_read_TS2,
        .i2c_address = TS2_ADDR,
        .config_register = 0,
    };

    mcp23018_I2C mcp23018 = {
        ._i2c_write = _mcp23018_I2C_write,
        ._i2c_read = _mcp23018_I2C_read,
        .i2c_address = MCP23018_ADDR,
        .iocon = 0,
        .dirRegisters = {0, 0},
        .polRegisters = {0, 0},
        .pullupRegisters = {0, 0},
        .ports = {0, 0},
    };

    tmp1075_init(&tmp1075_1);
    tmp1075_init(&tmp1075_2);
    mcp23018_init(&mcp23018, IOEXP_MODE);

    // set all pins to output
    mcp23018_set_port_mode(&mcp23018, PORT_A, ALL_OUTPUT);
    mcp23018_set_port_mode(&mcp23018, PORT_B, ALL_OUTPUT);

    // set all pins to low
    mcp23018_digital_write_port(&mcp23018, PORT_A, ALL_LOW);
    mcp23018_digital_write_port(&mcp23018, PORT_B, ALL_LOW);

    while (1) {
        int16_t raw;
        float temp;
        tmp1075_get_temp_raw(&tmp1075_1, &raw);
        tmp1075_get_temp_celsius(&tmp1075_1, &temp);
        printf("#TS1=> raw: %d, temp: %f\n", raw, temp);
        tmp1075_get_temp_raw(&tmp1075_2, &raw);
        tmp1075_get_temp_celsius(&tmp1075_2, &temp);
        printf("#TS2=> raw: %d, temp: %f\n", raw, temp);
        mcp23018_digital_write_port(&mcp23018, PORT_A, ALL_HIGH);
        mcp23018_digital_write_port(&mcp23018, PORT_B, ALL_HIGH);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        mcp23018_digital_write_port(&mcp23018, PORT_A, ALL_LOW);
        mcp23018_digital_write_port(&mcp23018, PORT_B, ALL_LOW);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
