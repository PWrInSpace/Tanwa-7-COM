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
    if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case 'F':
            ret = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver open valve FILL failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve open FILL");
            break;
        case 'D':
            ret = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver open valve DEPR failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve open DEPR");
            break;
        case 'A':
            ret = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver open valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve open ADD");
            break;
        default:
            ESP_LOGE(TAG, "Invalid solenoid valve identifier");
            break;
    }
    return 0;
}

static int close_solenoid(int argc, char **argv) {
     if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case 'F':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve FILL failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close FILL");
            break;
        case 'D':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve DEPR failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close DEPR");
            break;
        case 'A':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        default:
            ESP_LOGE(TAG, "Invalid solenoid valve identifier");
            break;
    }
    return 0;
}

static int toggle_solenoid(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case 'F':
            ret = solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver toggle valve FILL failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid driver toggle valve FILL");
            break;
        case 'D':
            ret = solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver toggle valve DEPR failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid driver toggle valve DEPR");
            break;
        case 'A':
            ret = solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver toggle valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid driver toggle valve ADD");
            break;
        default:
            ESP_LOGE(TAG, "Invalid solenoid valve identifier");
            break;
    }
    return 0;
}

static int arm_igniter(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case '1':
            ret = igniter_arm(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 arm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 armed");
            break;
        case '2':
            ret = igniter_arm(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 arm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 armed");
            break;
        case 'A':
            ret = igniter_arm(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 arm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 armed");
            ret = igniter_arm(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 arm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 armed");
            break;
        default:
            ESP_LOGE(TAG, "Invalid igniter identifier");
            break;
    }
    return 0;
}

static int disarm_igniter(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case '1':
            ret = igniter_disarm(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 disarm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 disarmed");
            break;
        case '2':
            ret = igniter_disarm(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 disarm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 disarmed");
            break;
        case 'A':
            ret = igniter_disarm(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 disarm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 disarmed");
            ret = igniter_disarm(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 disarm failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 disarmed");
            break;
        default:
            ESP_LOGE(TAG, "Invalid igniter identifier");
            break;
    }
    return 0;
}

static int fire_igniter(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case '1':
            ret = igniter_fire(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 fire failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 fired");
            break;
        case '2':
            ret = igniter_fire(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 fire failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 fired");
            break;
        case 'A':
            ret = igniter_fire(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 fire failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 fired");
            ret = igniter_fire(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 fire failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 fired");
            break;
        default:
            ESP_LOGE(TAG, "Invalid igniter identifier");
            break;
    }
    return 0;
}

static int reset_igniter(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case '1':
            ret = igniter_reset(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 reset failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 reset");
            break;
        case '2':
            ret = igniter_reset(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 reset failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 reset");
            break;
        case 'A':
            ret = igniter_reset(&(TANWA_hardware.igniter[0]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #1 reset failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #1 reset");
            ret = igniter_reset(&(TANWA_hardware.igniter[1]));
            if (ret != IGNITER_OK) {
                ESP_LOGE(TAG, "Igniter #2 reset failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Igniter #2 reset");
            break;
        default:
            ESP_LOGE(TAG, "Invalid igniter identifier");
            break;
    }
    return 0;
}

static int check_igniter_continuity(int argc, char **argv) {
    uint8_t ret = 0;
    igniter_continuity_t status;

    ret = igniter_check_continuity(&(TANWA_hardware.igniter[0]), &status);
    if (ret != IGNITER_OK) {
        ESP_LOGE(TAG, "Igniter #1 continuity check failed - status: %d", ret);
        return -1;
    }
    ret = igniter_check_continuity(&(TANWA_hardware.igniter[1]), &status);
    if (ret != IGNITER_OK) {
        ESP_LOGE(TAG, "Igniter #2 continuity check failed - status: %d", ret);
        return -1;
    }
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
    {"valve-open", "open solenoid valve", "F|D|A", open_solenoid, NULL},
    {"valve-close", "close solenoid valve", "F|D|A", close_solenoid, NULL},
    {"valve-toggle", "toggle solenoid valve", "F|D|A", toggle_solenoid, NULL},
    // igniter commands
    {"igniter-arm", "arm igniter", "1|2|A", arm_igniter, NULL},
    {"igniter-disarm", "disarm igniter", "1|2|A", disarm_igniter, NULL},
    {"igniter-fire", "fire igniter", "1|2|A", fire_igniter, NULL},
    {"igniter-reset", "reset igniter", "1|2|A", reset_igniter, NULL},
    {"igniter-continuity", "check igniters continuity", NULL, check_igniter_continuity, NULL},
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