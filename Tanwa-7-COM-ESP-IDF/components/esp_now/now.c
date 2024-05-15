#include "now.h"

// Adres OBC:
const uint8_t adressObc[] = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20};

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
    esp_wifi_start();

    if (esp_now_init())
        return false;

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

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

    // protection against unwanted state change in service mode (possibility of flashe erase)
    if (moduleData.inServiceMode == true) {
        return;
    }

    if (adressCompare(mac, adressObc)) {

        // if nextSendTime:
        if (len == sizeof(moduleData.obcState)) {

            memcpy((void*) &moduleData.obcState, (uint16_t *)incomingData, sizeof(moduleData.obcState));

            // if long time - go to sleep:
            if (stateInWakenUp()) moduleData.dataToObc.wakenUp = false;
            else moduleData.dataToObc.wakenUp = true;
        }

        // if different command:
        else rxNowHandler(incomingData, len);
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