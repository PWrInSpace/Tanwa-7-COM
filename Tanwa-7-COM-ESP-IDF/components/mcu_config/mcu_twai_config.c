// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_twai_config.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TAG "MCU_TWAI"

mcu_twai_config_t mcu_twai_config = {
    .tx_gpio_num = CONFIG_CAN_RX,
    .rx_gpio_num = CONFIG_CAN_TX,
    .mode = TWAI_MODE_NORMAL,
    .g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = CONFIG_CAN_TX,
        .rx_io = CONFIG_CAN_RX,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 10,
        .rx_queue_len = 10,
        .alerts_enabled = TWAI_ALERT_NONE, // for now - ToDo: change and test alerts
        .clkout_divider = 0,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    },
    .t_config = TWAI_TIMING_CONFIG_250KBITS(),
    .f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(),
};

esp_err_t mcu_twai_init() {
    esp_err_t err;
    err = twai_driver_install(&(mcu_twai_config.g_config), 
                              &(mcu_twai_config.t_config),
                              &(mcu_twai_config.f_config));
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "TWAI driver install error");
      return err;
    }
    return ESP_OK;
}

void mcu_twai_check_alerts() {
    // while (true) {
    uint32_t alerts;
    twai_read_alerts(&alerts, portMAX_DELAY);
    printf("%08x", alerts);
    if (alerts & TWAI_ALERT_ABOVE_ERR_WARN) {
        ESP_LOGI(TAG, "Surpassed Error Warning Limit");
    }
    if (alerts & TWAI_ALERT_ERR_PASS) {
        ESP_LOGI(TAG, "Entered Error Passive state");
    }
    if (alerts & TWAI_ALERT_BUS_OFF) {
        ESP_LOGI(TAG, "Bus Off state");
        // Prepare to initiate bus recovery, reconfigure alerts to detect bus recovery completion
        twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED, NULL);
        for (int i = 3; i > 0; i--) {
            ESP_LOGW(TAG, "Initiate bus recovery in %d", i);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        twai_initiate_recovery();    //Needs 128 occurrences of bus free signal
        ESP_LOGI(TAG, "Initiate bus recovery");
    }
    if (alerts & TWAI_ALERT_BUS_ERROR) {
        ESP_LOGI(TAG, "Bus Error state");
        // Prepare to initiate bus recovery, reconfigure alerts to detect bus recovery completion
        twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED, NULL);
        for (int i = 3; i > 0; i--) {
            ESP_LOGW(TAG, "Initiate bus recovery in %d", i);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        twai_initiate_recovery();    //Needs 128 occurrences of bus free signal
        ESP_LOGI(TAG, "Initiate bus recovery");
    }
    if (alerts & TWAI_ALERT_BUS_RECOVERED) {
      //Bus recovery was successful, exit control task to uninstall driver
      ESP_LOGI(TAG, "Bus Recovered");
      // break;
    }
}