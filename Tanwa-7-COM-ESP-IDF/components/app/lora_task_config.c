#include "lora_task_config.h"

#include "lora.pb-c.h"

#include "TANWA_config.h"
#include "mcu_gpio_config.h"
#include "mcu_spi_config.h"
#include "mcu_misc_config.h"

#include "cmd_commands.h"

#include "esp_log.h"

#define TAG "LORA_TASK_CONFIG"

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

    LoRaCommand* received =
        lo_ra_command__unpack(NULL, packet_size - prefix_size - 1, packet + prefix_size);
    if (received != NULL) {
        ESP_LOGI(TAG, "Received LORA_ID %d, DEV_ID %d, COMMAND %d, PLD %d", received->lora_dev_id,
                 received->sys_dev_id, received->command, received->payload);
        // cmd_message_t received_command = cmd_create_message(received->command, received->payload);
        lora_command_parsing(received->lora_dev_id, received->command, received->payload);
        lo_ra_command__free_unpacked(received, NULL);
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