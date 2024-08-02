#include "now.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#define TAG "NOW"

#define ESP_NOW_QUEUE_LENGTH 100

// Adress OBC:
const uint8_t adress_obc[] = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20};
// Adress TANWA:
const uint8_t adress_tanwa[] = {0x80, 0x08, 0x50, 0x80, 0x08, 0x50}; // BOOBS
// Adress MAIN_VALVE_1:
const uint8_t adress_main_valve_1[] = {0x80, 0x08, 0x50, 0x80, 0x08, 0x51};
// Adress MAIN_VALVE_2:
const uint8_t adress_main_valve_2[] = {0x80, 0x08, 0x50, 0x80, 0x08, 0x52};

QueueHandle_t obc_now_rx_queue = NULL;
QueueHandle_t main_valve_1_now_rx_queue = NULL;
QueueHandle_t main_valve_2_now_rx_queue = NULL;

// extern volatile ModuleData module_data;

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2);

/**********************************************************************************************/

bool nowInit() {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_mac(WIFI_IF_STA , adress_tanwa);
    esp_wifi_start();
    // init esp_now
    if (esp_now_init())
        return false;
    // init esp_now rx queue
    obc_now_rx_queue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(DataFromObc));
    main_valve_1_now_rx_queue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(DataFromMainValve1));
    main_valve_2_now_rx_queue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(DataFromMainValve2));
    // register callbacks
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    return true;
}

/**********************************************************************************************/

bool nowAddPeer(const uint8_t *address, uint8_t channel) {

    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr, address, 6);
    peerInfo.channel = channel;

    if (esp_now_add_peer(&peerInfo))
        return false;
    return true;
}

/**********************************************************************************************/

// This callback is not used in Broadcast concept. It is left for future ideas.
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

/**********************************************************************************************/

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    uint8_t obcState;
    DataFromObc obc_data;
    DataFromMainValve1 main_valve_1_data;
    DataFromMainValve2 main_valve_2_data;

    if (adressCompare(info->src_addr, adress_obc)) {
        if (len == sizeof(obcState)) {
            memcpy((void*) &obcState, (uint16_t *)incomingData, sizeof(obcState));
            ESP_LOGI(TAG, "OBC state: %d", obcState);
        } else {
            memcpy((void*) &obc_data, incomingData, sizeof(DataFromObc));
            if (xQueueSendToBack(obc_now_rx_queue, &obc_data, 0) != pdTRUE) {
                ESP_LOGE(TAG, "ESP-NOW RX queue error sending to back");
            }
        }
    }

    if (adressCompare(info->src_addr, adress_main_valve_1)) {
        memcpy((void*) &main_valve_1_data, incomingData, sizeof(DataFromMainValve1));
        if (xQueueSendToBack(main_valve_1_now_rx_queue, &main_valve_1_data, 0) != pdTRUE) {
            ESP_LOGE(TAG, "ESP-NOW RX queue error sending to back");
        }
    }

    if (adressCompare(info->src_addr, adress_main_valve_2)) {
        memcpy((void*) &main_valve_2_data, incomingData, sizeof(DataFromMainValve2));
        if (xQueueSendToBack(main_valve_2_now_rx_queue, &main_valve_2_data, 0) != pdTRUE) {
            ESP_LOGE(TAG, "ESP-NOW RX queue error sending to back");
        }
    }
}

/**********************************************************************************************/

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2) {

    for (int8_t i = 0; i < 6; i++) {

        if (addr1[i] != addr2[i])
            return false;
    }

    return true;
}