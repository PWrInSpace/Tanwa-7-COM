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

#include "cmd_commands.h"

#include "esp_log.h"

#define TAG "LORA_TASK"

#define LORA_TASK_STACK_SIZE 8192
#define LORA_TASK_PRIORITY 2
#define LORA_TASK_CORE 0

static struct {
    lora_struct_t lora;
    lora_task_process_rx_packet process_packet_fnc;
    lora_task_get_tx_packet get_tx_packet_fnc;
    lora_state_t lora_state;
    uint8_t tx_buffer[256];
    size_t tx_buffer_size;

    TimerHandle_t receive_window_timer;
    TaskHandle_t task;
} gb;

static bool wait_until_irq(void) {
    return ulTaskNotifyTake(pdTRUE, portMAX_DELAY) == pdTRUE ? true : false;
}



void IRAM_ATTR lora_task_irq_notify(void *arg) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    vTaskNotifyGiveFromISR(gb.task, &higher_priority_task_woken);
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void notify_end_of_rx_window(void) { 
    xTaskNotifyGive(gb.task);
    ESP_LOGI(TAG, "END OF WINDOW");
}

static void on_receive_window_timer(TimerHandle_t timer) { notify_end_of_rx_window(); }

static void lora_change_state_to_receive() {
    ESP_LOGD(TAG, "Changing state to receive");
    if (gb.lora_state == LORA_RECEIVE) {
        return;
    }

    lora_map_d0_interrupt(&gb.lora, LORA_IRQ_D0_RXDONE);
    lora_set_receive_mode(&gb.lora);
    gb.lora_state = LORA_RECEIVE;
}

static void lora_change_state_to_transmit() {
    ESP_LOGD(TAG, "Changing state to transmit");
    if (gb.lora_state == LORA_TRANSMIT) {
        return;
    }

    gb.lora_state = LORA_TRANSMIT;
}

void turn_on_receive_window_timer(void) {
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        xTimerReset(gb.receive_window_timer, portMAX_DELAY);
        ESP_LOGE(TAG, "TIMER IS ACTIVE");
        return;
    }
    xTimerStart(gb.receive_window_timer, portMAX_DELAY);
}

void turn_of_receive_window_timer(void) {
    if (xTimerIsTimerActive(gb.receive_window_timer) == pdTRUE) {
        xTimerStop(gb.receive_window_timer, portMAX_DELAY);
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

static void transmint_packet(void) {
    if (gb.get_tx_packet_fnc == NULL) {
        return;
    }

    gb.tx_buffer_size = gb.get_tx_packet_fnc(gb.tx_buffer, sizeof(gb.tx_buffer));
    lora_send_packet(&gb.lora, gb.tx_buffer, gb.tx_buffer_size);
}

static void on_lora_transmit() {
    lora_change_state_to_receive();
    turn_of_receive_window_timer();
    turn_on_receive_window_timer();
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
static size_t add_prefix(uint8_t* buffer, size_t size) {
    if (size < 6) {
        return 0;
    }

    memcpy(buffer, PACKET_PREFIX, sizeof(PACKET_PREFIX) - 1);

    return sizeof(PACKET_PREFIX) - 1;
}

// static size_t lora_create_settings_packet(uint8_t* buffer, size_t size) {
//     LoRaSettings frame = LO_RA_SETTINGS__INIT;
//     create_protobuf_settings_frame(&frame);

//     uint8_t data_size = 0;
//     uint8_t prefix_size = 0;
//     prefix_size = add_prefix(buffer, size);
//     data_size = lo_ra_settings__pack(&frame, buffer + prefix_size);

//     return prefix_size + data_size;
// }
#include "TANWA_data.h"
void create_porotobuf_data_frame(LoRaFrame *frame) {
    tanwa_data_t tanwa_data = tanwa_data_read();
    lo_ra_frame__init(frame);   // fill struct with 0
    // mcb
   // frame->obc_state = data.mcb.state;
   frame->tanwa_state = tanwa_data.state;
   frame->pressure_injector_fuel = tanwa_data.com_data.pressure_1;
   frame->pressure_injector_oxi = tanwa_data.com_data.pressure_2;
   frame->pressure_combustion_chamber = tanwa_data.com_data.pressure_3;
   frame->igniter_cont1 = tanwa_data.com_data.igniter_cont_1;
   frame->igniter_cont2 = tanwa_data.com_data.igniter_cont_2;
   frame->ststus_oxy= tanwa_data.com_liquid_data.solenoid_state_oxy;
   frame->status_fuel = tanwa_data.com_liquid_data.solenoid_state_fuel;
   frame->status_arm = tanwa_data.com_liquid_data.arm_state;
   frame->tanwa_battery = tanwa_data.com_data.vbat;
   frame->temp_injector = tanwa_data.can_flc_data.temperature_1;
   frame->temp_combustion_chamber = tanwa_data.can_flc_data.temperature_2;
}

static size_t lora_create_data_packet(uint8_t* buffer, size_t size) {
    LoRaFrame frame;
    create_porotobuf_data_frame(&frame);

    uint8_t data_size = 0;
    uint8_t prefix_size = 0;
    prefix_size = add_prefix(buffer, size);
    data_size = lo_ra_frame__pack(&frame, buffer + prefix_size);

    return prefix_size + data_size;
}

static size_t lora_packet(uint8_t* buffer, size_t buffer_size) 
{   size_t size = 0;
    size = lora_create_data_packet(buffer, buffer_size);
    ESP_LOGI(TAG, "Sending LoRa frame -> size: %d", size);

    return size;
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
        .get_tx_packet_fnc = lora_packet,
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

    if (cfg->process_rx_packet_fnc == NULL || cfg->get_tx_packet_fnc == NULL) {
        return false;
    }

    gb.process_packet_fnc = cfg->process_rx_packet_fnc;
    gb.get_tx_packet_fnc = cfg->get_tx_packet_fnc;
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

    gb.receive_window_timer =
        xTimerCreate("Transmit timer", pdMS_TO_TICKS(cfg->transmiting_period), pdFALSE, NULL,
                     on_receive_window_timer);
    ESP_LOGD(TAG, "Starting timer");
    lora_change_state_to_receive();
    turn_on_receive_window_timer();

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

bool lora_change_frequency(uint32_t frequency_khz) 
{
    if (frequency_khz < 4e5 || frequency_khz > 1e6) {
        return false;
    }
    if (lora_set_frequency(&gb.lora, frequency_khz * 1000) != LORA_OK) {
        return false;
    }
    return true;
}


void lora_task(void *arg) 
{
    uint8_t rx_buffer[256];
    size_t rx_packet_size = 0;

    while (1) {
        if (wait_until_irq() == true) {
            // on transmit
            if (gb.lora_state == LORA_TRANSMIT) {
                ESP_LOGI(TAG, "ON transmit");
                on_lora_transmit();
            // on receive
            } else {
                rx_packet_size = on_lora_receive(rx_buffer, sizeof(rx_buffer));
                if (rx_packet_size > 0 && gb.process_packet_fnc != NULL) {
                    gb.process_packet_fnc(rx_buffer, rx_packet_size);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                lora_change_state_to_transmit();
                transmint_packet();
                // qucik fix
                turn_on_receive_window_timer();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
