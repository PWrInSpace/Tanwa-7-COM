///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 14.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#include "console_config.h"

#include "esp_log.h"
#include "esp_system.h"

#include "console.h"

#include "TANWA_config.h"
#include "mcu_adc_config.h"

#define TAG "CONSOLE_CONFIG"

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

static int reset_device(int argc, char **arg) {
    esp_restart();
    return 0;
}

static int enable_log(int argc, char **argv) {
    if (argc == 2) {
        esp_log_level_set(argv[1], ESP_LOG_DEBUG);
    } else {
        esp_log_level_set("*", ESP_LOG_DEBUG);
    }
    // esp_log_level_set("spi_master", ESP_LOG_ERROR);
    return 0;
}

static int disable_log(int argc, char **argv) {
    if (argc == 2) {
        esp_log_level_set(argv[1], ESP_LOG_WARN);
    } else {
        esp_log_level_set("*", ESP_LOG_WARN);
    }

    return 0;
}

// static int force_change_state(int argc, char **argv) {
//     if (argc != 2) {
//         return -1;
//     }

//     int state = atoi(argv[1]);
//     if (state == 11) {
//         if (SM_get_current_state() == HOLD) {
//             ESP_LOGI(TAG, "Leaving hold state");
//             if (SM_get_previous_state() == COUNTDOWN) {
//                 SM_force_change_state(RDY_TO_LAUNCH);
//             } else {
//                 SM_change_to_previous_state(true);
//             }
//         } else {
//             SM_force_change_state(HOLD);
//             ESP_LOGI(TAG, "HOLD");
//         }
//         return 0;
//     }
//     if (SM_force_change_state(state) != SM_OK) {
//         return -1;
//     }

//     return 0;
// }

// static int change_to_previous_state(int argc, char **argv) {
//     bool run_callback = false;
//     if (argc == 2) {
//         run_callback = true;
//     }

//     if (SM_change_to_previous_state(run_callback) != SM_OK) {
//         return -1;
//     }

//     return 0;
// }

// static int get_state(int argc, char **argv) {
//     CONSOLE_WRITE_G("Current state -> %d", SM_get_current_state());
//     return 0;
// }

static int read_temperature(int argc, char **argv) {
    int16_t raw[2];
    float temp[2];
    uint8_t ret = 0;

    ret = tmp1075_get_temp_raw(&(TANWA_hardware.tmp1075[0]), &raw[0]);
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "TMP1075 #1 read raw failed - status: %d", ret);
        return -1;
    }
    ret = tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[0]), &temp[0]);
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "TMP1075 #1 read temp failed - status: %d", ret);
        return -1;
    }
    ret = tmp1075_get_temp_raw(&(TANWA_hardware.tmp1075[1]), &raw[1]);
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "TMP1075 #2 read raw failed - status: %d", ret);
        return -1;
    }
    ret = tmp1075_get_temp_celsius(&(TANWA_hardware.tmp1075[1]), &temp[1]);
    if (ret != TMP1075_OK) {
        ESP_LOGE(TAG, "TMP1075 #2 read temp failed - status: %d", ret);
        return -1;
    }
    
    CONSOLE_WRITE("TMP1075 Temperature:");
    CONSOLE_WRITE("#1 => raw: %d, temp: %f", raw[0], temp[0]);
    CONSOLE_WRITE("#2 => raw: %d, temp: %f", raw[1], temp[1]);

    return 0;
}

static int read_pressure(int argc, char **argv) {
    float voltage[4], pressure[4];
    uint8_t ret = 0;

    ret = pressure_driver_read_voltage(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &voltage[0]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read voltage failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_1, &pressure[0]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read pressure failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_voltage(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &voltage[1]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read voltage failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_2, &pressure[1]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read pressure failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_voltage(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_3, &voltage[2]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read voltage failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_3, &pressure[2]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read pressure failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_voltage(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_4, &voltage[3]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read voltage failed - status: %d", ret);
        return -1;
    }
    ret = pressure_driver_read_pressure(&(TANWA_utility.pressure_driver), PRESSURE_DRIVER_SENSOR_4, &pressure[3]);
    if (ret != PRESSURE_DRIVER_OK) {
        ESP_LOGE(TAG, "Pressure driver read pressure failed - status: %d", ret);
        return -1;
    }

    CONSOLE_WRITE("ADS1115 Pressure:");
    CONSOLE_WRITE("#1 => voltage: %f, pressure: %f", voltage[0], pressure[0]);
    CONSOLE_WRITE("#2 => voltage: %f, pressure: %f", voltage[1], pressure[1]);
    CONSOLE_WRITE("#3 => voltage: %f, pressure: %f", voltage[2], pressure[2]);
    CONSOLE_WRITE("#4 => voltage: %f, pressure: %f", voltage[3], pressure[3]);

    return 0;
}

static int read_vbat(int argc, char **argv) {
    float voltage;
    bool ret = true;

    ret = mcu_adc_read_voltage(VBAT_CHANNEL_INDEX, &voltage);
    if (ret != true) {
        ESP_LOGE(TAG, "VBAT read voltage failed");
        return -1;
    }

    CONSOLE_WRITE("VBAT Voltage:");
    CONSOLE_WRITE("voltage: %f", voltage);

    return 0;
}

static int open_solenoid(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }
    uint8_t ret = 0;

    

    return 0;
}

static int close_solenoid(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static int toggle_solenoid(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static int arm_igniter(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static int disarm_igniter(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static int fire_igniter(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static int reset_igniter(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static int check_igniter_continuity(int argc, char **argv) {
    uint8_t ret = 0;
    // TODO: implement
    return 0;
}

static esp_console_cmd_t cmd[] = {
    // system commands
    {"reset-dev", "restart device", NULL, reset_device, NULL},
    {"log-enable", "enable logs", NULL, enable_log, NULL},
    {"log-disable", "disable logs", NULL, disable_log, NULL},
    // // state machine commands
    // {"state-change", "change state", NULL, force_change_state, NULL},
    // {"state-change-prev", "change state to previous", NULL, change_to_previous_state, NULL},
    // {"state-change-force", "force change state", NULL, force_change_state, NULL},
    // {"state-get", "get current state", NULL, get_state, NULL},
    // measurements commands
    {"temp-read", "read temperature", NULL, read_temperature, NULL},
    {"pressure-read", "read pressure", NULL, read_pressure, NULL},
    {"vbat-read", "read vbat voltage", NULL, read_vbat, NULL},
    // solenoid valve commands
    {"valve-open", "open solenoid valve", NULL, open_solenoid, NULL},
    {"valve-close", "close solenoid valve", NULL, close_solenoid, NULL},
    {"valve-toggle", "toggle solenoid valve", NULL, toggle_solenoid, NULL},
    // igniter commands
    {"igniter-arm", "arm igniter", NULL, arm_igniter, NULL},
    {"igniter-disarm", "disarm igniter", NULL, disarm_igniter, NULL},
    {"igniter-fire", "fire igniter", NULL, fire_igniter, NULL},
    {"igniter-reset", "reset igniter", NULL, reset_igniter, NULL},
    {"igniter-continuity", "check igniter continuity", NULL, check_igniter_continuity, NULL},
};

esp_err_t console_config_init() {
    esp_err_t ret;
    ret = console_init();
    ret = console_register_commands(cmd, sizeof(cmd) / sizeof(cmd[0]));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(ret));
        return ret;
    }
    return ret;
}