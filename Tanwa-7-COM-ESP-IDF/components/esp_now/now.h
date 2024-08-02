#ifndef PWRINSPACE_NOW_H
#define PWRINSPACE_NOW_H

#include <stdbool.h>
#include <stdint.h>

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "esp_crc.h"

#include "now_structs.h"

#define NOW_GET_DATA 0x01

extern const uint8_t adress_obc[];
extern const uint8_t adress_main_valve_1[];
extern const uint8_t adress_main_valve_2[];
extern QueueHandle_t obc_now_rx_queue;
extern QueueHandle_t main_valve_1_now_rx_queue;
extern QueueHandle_t main_valve_2_now_rx_queue;

// Init:
bool nowInit();

// Dodanie peera:
bool nowAddPeer(const uint8_t* address, uint8_t channel);

// Przerwania:
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);

#endif