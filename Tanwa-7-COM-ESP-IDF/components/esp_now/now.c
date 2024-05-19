#include "now.h"

#include <string.h>

#include "now_structs.h"

// Adress OBC:
const uint8_t adress_obc[] = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20};
// Adress TANWA:
const uint8_t adress_tanwa[] = {0x80, 0x08, 0x50, 0x80, 0x08, 0x50}; // BOOBS

extern volatile ModuleData module_data;

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
    // protection against unwanted state change in service mode (possibility of flashe erase)
    if (module_data.inServiceMode == true) {
        return;
    }
    if (adressCompare(info->src_addr, adress_obc)) {
        // if nextSendTime:
        if (len == sizeof(module_data.obcState)) {
            memcpy((void*) &module_data.obcState, (uint16_t *)incomingData, sizeof(module_data.obcState));
        }
        // if different command:
        // else rxNowHandler(incomingData, len);
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