// Copyright 2022 PWrInSpace, Kuba
#include "lora_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "lora.pb-c.h"

#include "TANWA_config.h"
#include "mcu_gpio_config.h"
#include "mcu_spi_config.h"
#include "mcu_misc_config.h"
#include "timers_config.h"

#include "cmd_commands.h"
#include "system_timer.h"

#include "esp_log.h"

#define TAG "LORA_TASK"

#define LORA_TASK_STACK_SIZE 8192
#define LORA_TASK_PRIORITY 2
#define LORA_TASK_CORE 0

static struct {
    lora_struct_t lora;
    lora_task_process_rx_packet process_packet_fnc;
    TaskHandle_t task;
} gb;

void IRAM_ATTR lora_task_irq_notify(void *arg) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    vTaskNotifyGiveFromISR(gb.task, &higher_priority_task_woken);
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static size_t on_lora_receive(uint8_t *rx_buffer, size_t buffer_len) {
    size_t len = 0;
    if (lora_received(&gb.lora) == LORA_OK) {
        len = lora_receive_packet(&gb.lora, rx_buffer, buffer_len);
        rx_buffer[len] = '\0';
        ESP_LOGD(TAG, "Received %s, len %d", rx_buffer, len);
        lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
        lora_set_receive_mode(&gb.lora);
    }
    return len;
}

static bool check_prefix(uint8_t* packet, size_t packet_size) {
    if (packet_size < sizeof(PACKET_PREFIX)) {
        return false;
    }

    uint8_t prefix[] = PACKET_PREFIX;
    for (int i = 0; i < sizeof(PACKET_PREFIX) - 1; ++i) {
        if (packet[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}

static uint8_t calculate_checksum(uint8_t* buffer, size_t size) {
    uint8_t sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += buffer[i];
    }

    return sum;
}

static void lora_process(uint8_t* packet, size_t packet_size) {
    if (packet_size > 40) {
        ESP_LOGI(TAG, "Recevied packet is too big");
        // errors_set(ERROR_TYPE_LAST_EXCEPTION, ERROR_EXCP_LORA_DECODE, 100);
        return;
    }

    if (check_prefix(packet, packet_size) == false) {
        ESP_LOGE(TAG, "LoRa invalid prefix");
        return;
    }


    uint8_t prefix_size = sizeof(PACKET_PREFIX) - 1;
    if (calculate_checksum(packet + prefix_size, packet_size - prefix_size - 1) != packet[packet_size - 1]) {
        ESP_LOGE(TAG, "Invalid checksum");
        return;
    }

    ESP_LOGI(TAG, "Received packet: %s", packet + prefix_size);

    LoRaCommand* received =
        lo_ra_command__unpack(NULL, packet_size - prefix_size - 1, packet + prefix_size);
    if (received != NULL) {
        ESP_LOGI(TAG, "Received LORA_ID %d, DEV_ID %d, COMMAND %d, PLD %d", received->lora_dev_id,
                 received->sys_dev_id, received->command, received->payload);
        // cmd_message_t received_command = cmd_create_message(received->command, received->payload);
        if(lora_command_parsing(received->lora_dev_id, received->command, received->payload) == false) {
            ESP_LOGE(TAG, "Unable to prcess command :C");
            return;
        }
        lo_ra_command__free_unpacked(received, NULL);

        if (sys_timer_restart(TIMER_DISCONNECT, TIMER_DISCONNECT_PERIOD_MS) == false) {
            ESP_LOGE(TAG, "Unable to restart timer");
        }
    } else {
        ESP_LOGE(TAG, "Unable to decode received package");
    }
}

bool initialize_lora(uint32_t frequency_khz, uint32_t transmiting_period) {
    _lora_add_device();
    _lora_gpio_attach_d0_isr(lora_task_irq_notify);
    lora_struct_t lora = {
        ._spi_transmit = _lora_spi_transmit,
        ._delay = _lora_delay_ms,
        ._gpio_set_level = _lora_gpio_set_level,
        .log = _lora_log,
        .rst_gpio_num = CONFIG_LORA_RS,
        .cs_gpio_num = CONFIG_LORA_CS,
        .d0_gpio_num = CONFIG_LORA_D0,
        .implicit_header = 0,
        .frequency = 0,
    };
    lora_api_config_t cfg = {
        .lora = &lora,
        .process_rx_packet_fnc = lora_process,
        .frequency_khz = frequency_khz,
        .transmiting_period = transmiting_period,
    };
    lora_task_init(&cfg);
    return true;
}

bool lora_task_init(lora_api_config_t *cfg) {
    assert(cfg != NULL);
    if (cfg == NULL) {
        return false;
    }

    if (cfg->process_rx_packet_fnc == NULL) {
        return false;
    }

    gb.process_packet_fnc = cfg->process_rx_packet_fnc;
    memcpy(&gb.lora, cfg->lora, sizeof(lora_struct_t));

    lora_init(&gb.lora);
    lora_set_frequency(&gb.lora, cfg->frequency_khz * 1e3);
    lora_set_bandwidth(&gb.lora, LORA_TASK_BANDWIDTH);
    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    lora_set_receive_mode(&gb.lora);

    if (LORA_TASK_CRC_ENABLE) {
        lora_enable_crc(&gb.lora);
    } else {
        lora_disable_crc(&gb.lora);
    }

    ESP_LOGI(TAG, "Reading LoRa registers");
    int16_t read_val_one = lora_read_reg(&gb.lora, 0x0d);
    int16_t read_val_two = lora_read_reg(&gb.lora, 0x0c);
    ESP_LOGI(TAG, "LORA_READ: %04x, %04x", read_val_one, read_val_two);

    xTaskCreatePinnedToCore(lora_task, "LoRa task", LORA_TASK_STACK_SIZE, NULL, LORA_TASK_PRIORITY,
                            &gb.task, LORA_TASK_CORE);

    if (gb.task == NULL) {
        return false;
    }
    return true;
}

bool lora_change_frequency(uint32_t frequency_khz) {
    if (frequency_khz < 4e5 || frequency_khz > 1e6) {
        return false;
    }
    if (lora_set_frequency(&gb.lora, frequency_khz * 1000) != LORA_OK) {
        return false;
    }
    return true;
}

void lora_task(void* pvParameters) {
    ESP_LOGI(TAG, "LoRa Task started");

    uint8_t rx_buffer[256];
    size_t rx_packet_size = 0;

    while (1) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "IRQ received");
            if (lora_received(&gb.lora) == LORA_OK) {
                rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));
                if (rx_packet_size > 0 && gb.process_packet_fnc != NULL) {
                    ESP_LOGI(TAG, "Processing packet");
                    gb.process_packet_fnc(rx_buffer, rx_packet_size);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}