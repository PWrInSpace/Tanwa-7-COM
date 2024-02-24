#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

#include "mcu_adc_config.h"

#include "TANWA_config.h"

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

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

    TANWA_mcu_config_init();
    TANWA_hardware_init();
    TANWA_utility_init();

    // mcu_twai_self_test();

    while (1) {
        int16_t raw;
        float temp, voltage, pressure;
        tmp1075_get_temp_raw(&(TANWA_hardware.tmp1075[0]), &raw);
        tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[0]), &temp);
        printf("#TS1=> raw: %d, temp: %f\n", raw, temp);
        tmp1075_get_temp_raw(&(TANWA_hardware.tmp1075[1]), &raw);
        tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[1]), &temp);
        printf("#TS2=> raw: %d, temp: %f\n", raw, temp);
        pressure_driver_read_voltage(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &voltage);
        pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure);
        printf("#ADC=> voltage: %f, pressure: %f\n", voltage, pressure);
        mcu_adc_read_voltage(VBAT_CHANNEL_INDEX, &voltage);
        printf("#VBAT=> voltage: %f\n", voltage);
        if (TANWA_utility.led_state_display.state == LED_STATE_DISPLAY_STATE_IDLE) {
            led_state_display_state_update(&(TANWA_utility.led_state_display), LED_STATE_DISPLAY_STATE_ARMED);
        } else {
            led_state_display_state_update(&(TANWA_utility.led_state_display), LED_STATE_DISPLAY_STATE_IDLE);
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
        solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
        solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
        led_toggle(&(TANWA_hardware.esp_led));
        vTaskDelay(500 / portTICK_PERIOD_MS);
        if (TANWA_hardware.igniter[0].state == IGNITER_STATE_WAITING) {
            igniter_continuity_t continuity;
            igniter_check_continuity(&(TANWA_hardware.igniter[0]), &continuity);
            igniter_arm(&(TANWA_hardware.igniter[0]));
            vTaskDelay(100 / portTICK_PERIOD_MS);
            igniter_fire(&(TANWA_hardware.igniter[0]));
        } else {
            igniter_disarm(&(TANWA_hardware.igniter[0]));
            igniter_reset(&(TANWA_hardware.igniter[0]));
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        if (TANWA_hardware.igniter[1].state == IGNITER_STATE_WAITING) {
            igniter_continuity_t continuity;
            igniter_check_continuity(&(TANWA_hardware.igniter[1]), &continuity);
            igniter_arm(&(TANWA_hardware.igniter[1]));
            vTaskDelay(100 / portTICK_PERIOD_MS);
            igniter_fire(&(TANWA_hardware.igniter[1]));
        } else {
            igniter_disarm(&(TANWA_hardware.igniter[1]));
            igniter_reset(&(TANWA_hardware.igniter[1]));
        }
    }
}
