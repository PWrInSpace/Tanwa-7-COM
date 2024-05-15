#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sys/time.h>
#include <string.h>

#include "dataStructs.h"
#include "now.h"
#include "driver/spi_master.h"
#include "driver/sdspi_host.h"
#include "esp_timer.h"
#include "cmd.h"

// To implementation:
void initPeripherals();
void measure();
void rxNowHandler(const uint8_t *incomingData, int len);
bool can_write_to_flash(void);
bool can_write_to_sd(void);
bool create_sd_string(DataToSave *data, char *buffer, size_t len);

// Not to touch:
void initDataStructsAndUart();
uint32_t getPowerTime_ms();
void goToSleep();
bool stateInWakenUp();
void checkSleepThings();
uint32_t getTimeMS();
bool initializeSPI(void);
void memory_check_start_condition(void);
void memory_check_end_condition(void);
void terminate_task_on_error(bool fail_cond, char *mess);
#endif