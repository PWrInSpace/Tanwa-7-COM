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
#include "TANWA_data.h"
#include "mcu_adc_config.h"
#include "mcu_twai_config.h"
#include "state_machine_config.h"

#include "measure_task.h"

#define TAG "CONSOLE_CONFIG"

extern TANWA_hardware_t TANWA_hardware;
extern TANWA_utility_t TANWA_utility;

static int reset_device(int argc, char **arg) {
    esp_restart();
    return 0;
}

static int reset_hx_rck(int argc, char **arg) {
    const twai_message_t hx_oxi_mess = {
        .identifier = 0x0A9, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int reset_hx_oxi(int argc, char **arg) {
    const twai_message_t hx_oxi_mess = {
        .identifier = 0x0B9, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100));
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

static int force_change_state(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    int state = atoi(argv[1]);
    if (state == 11) {
        if (state_machine_get_current_state() == HOLD) {
            ESP_LOGI(TAG, "Leaving hold state");
            if (state_machine_get_previous_state() == COUNTDOWN) {
                state_machine_force_change_state(RDY_TO_LAUNCH);
            } else {
                state_machine_change_to_previous_state(true);
            }
        } else {
            state_machine_force_change_state(HOLD);
            ESP_LOGI(TAG, "HOLD");
        }
        return 0;
    }
    if (state_machine_force_change_state(state) != STATE_MACHINE_OK) {
        return -1;
    }

    return 0;
}

static int change_to_previous_state(int argc, char **argv) {
    bool run_callback = false;
    if (argc == 2) {
        run_callback = true;
    }

    if (state_machine_change_to_previous_state(run_callback) != STATE_MACHINE_OK) {
        return -1;
    }

    return 0;
}

static int get_state(int argc, char **argv) {
    CONSOLE_WRITE_G("Current state -> %d", state_machine_get_current_state());
    return 0;
}

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

    ret = _mcu_adc_read_voltage(VBAT_CHANNEL_INDEX, &voltage);
    if (ret != true) {
        ESP_LOGE(TAG, "VBAT read voltage failed");
        return -1;
    }

    voltage = voltage * 6.26335877863f; // (10k + 50k) / 10k (voltage divider)

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
        case 'f':
            ret = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver open valve FILL failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve open FILL");
            break;
        case 'd':
            ret = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver open valve DEPR failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve open DEPR");
            break;
        case 'a':
            ret = solenoid_driver_valve_open(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver open valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve open ADD");
            break;
        case 'l':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FUEL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'o':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_OXY);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'v':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_VENT);
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

static int close_solenoid(int argc, char **argv) {
     if (argc < 2) {
        return -1;
    }
    uint8_t ret = 0;

    switch (*argv[1]) {
        case 'f':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve FILL failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close FILL");
            break;
        case 'd':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve DEPR failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close DEPR");
            break;
        case 'a':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'l':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FUEL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'o':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_OXY);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'v':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_VENT);
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
        case 'f':
            ret = solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FILL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver toggle valve FILL failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid driver toggle valve FILL");
            break;
        case 'd':
            ret = solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_DEPR);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver toggle valve DEPR failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid driver toggle valve DEPR");
            break;
        case 'a':
            ret = solenoid_driver_valve_toggle(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_ADD);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver toggle valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid driver toggle valve ADD");
            break;
        case 'l':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_FUEL);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'o':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_OXY);
            if (ret != SOLENOID_DRIVER_OK) {
                ESP_LOGE(TAG, "Solenoid driver close valve ADD failed - status: %d", ret);
                return -1;
            }
            CONSOLE_WRITE("Solenoid valve close ADD");
            break;
        case 'v':
            ret = solenoid_driver_valve_close(&(TANWA_utility.solenoid_driver), SOLENOID_DRIVER_VALVE_VENT);
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
        case 'a':
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
        case 'a':
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
        case 'a':
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
        case 'a':
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

static int qd_pull(int argc, char **argv) {
    const twai_message_t fac_mess = {
        .identifier = 0x0C1, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&fac_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int qd_stop(int argc, char **argv) {
    const twai_message_t fac_mess = {
        .identifier = 0x0C2,
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&fac_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int qd_push(int argc, char **argv) {
    const twai_message_t fac_mess = {
        .identifier = 0x0C3,
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&fac_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int fac_servo_open(int argc, char **argv){
    const twai_message_t fac_mess = {
        .identifier = 0x0C4,
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&fac_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int fac_servo_close(int argc, char **argv){
    const twai_message_t fac_mess = {
        .identifier = 0x0C5,
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&fac_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int fac_servo_open_angle(int argc, char **argv){
    if (argc < 2) {
        return -1;
    }
    uint16_t angle = atoi(argv[1]);
    twai_message_t fac_mess = {
        .identifier = 0x0C6,
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(fac_mess.data, &angle, sizeof(uint16_t));
    twai_transmit(&fac_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_rck_tare(int argc, char **argv) {
    const twai_message_t hx_rck_mess = {
        .identifier = 0x0A2, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_rck_calibrate(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float calib = atoi(argv[1]);
    twai_message_t hx_rck_mess = {
        .identifier = 0x0A3, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(hx_rck_mess.data, &calib, sizeof(float));
    twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_rck_set_calib_factor(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float calib = atoi(argv[1]);
    twai_message_t hx_rck_mess = {
        .identifier = 0x0A4, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(hx_rck_mess.data, &calib, sizeof(float));
    twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_rck_set_offset(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float offset = atoi(argv[1]);
    twai_message_t hx_rck_mess = {
        .identifier = 0x0A5, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(hx_rck_mess.data, &offset, sizeof(float));
    twai_transmit(&hx_rck_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_oxi_tare(int argc, char **argv) {
    const twai_message_t hx_oxi_mess = {
        .identifier = 0x0B2, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_oxi_calibrate(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float calib = atoi(argv[1]);
    twai_message_t hx_oxi_mess = {
        .identifier = 0x0B3, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(hx_oxi_mess.data, &calib, sizeof(float));
    twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_oxi_set_calib_factor(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float calib = atoi(argv[1]);
    twai_message_t hx_oxi_mess = {
        .identifier = 0x0B4, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(hx_oxi_mess.data, &calib, sizeof(float));
    twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int hx_oxi_set_offset(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float offset = atoi(argv[1]);
    twai_message_t hx_oxi_mess = {
        .identifier = 0x0B5, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(hx_oxi_mess.data, &offset, sizeof(float));
    twai_transmit(&hx_oxi_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int termo_heat_start(int argc, char **argv) {
    const twai_message_t termo_mess = {
        .identifier = 0x0E2, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&termo_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int termo_heat_stop(int argc, char **argv) {
    const twai_message_t termo_mess = {
        .identifier = 0x0E3, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    twai_transmit(&termo_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int termo_set_max_pressure(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float pressure = atoi(argv[1]);
    twai_message_t termo_mess = {
        .identifier = 0x0E6, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(termo_mess.data, &pressure, sizeof(float));
    twai_transmit(&termo_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int termo_set_min_pressure(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    float pressure = atoi(argv[1]);
    twai_message_t termo_mess = {
        .identifier = 0x0E7, 
        .data_length_code = 0,                  
        .data = {0, 0, 0, 0, 0, 0, 0, 0} 
    };
    memcpy(termo_mess.data, &pressure, sizeof(float));
    twai_transmit(&termo_mess, pdMS_TO_TICKS(100));
    return 0;
}

static int change_measure_period(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }

    uint32_t period = atoi(argv[1]);
    change_measure_task_period(period);

    return 0;
}

static int get_tanwa_data(int argc, char **argv) {
    tanwa_data_t tanwa_data = tanwa_data_read();
    CONSOLE_WRITE("TANWA Data:");
    CONSOLE_WRITE("STATE: %d", tanwa_data.state);
    CONSOLE_WRITE("COM: ");
    CONSOLE_WRITE("  Vbat: %.2f", tanwa_data.com_data.vbat);
    CONSOLE_WRITE("  Abort button: %d", tanwa_data.com_data.abort_button);
    CONSOLE_WRITE("  Valves: [F] %d, [D] %d", tanwa_data.com_data.solenoid_state_fill, tanwa_data.com_data.solenoid_state_depr);
    CONSOLE_WRITE("  Pressures: [0] %.2f, [1] %.2f, [2] %.2f, [3] %.2f",
                  tanwa_data.com_data.pressure_1, tanwa_data.com_data.pressure_2,
                  tanwa_data.com_data.pressure_3, tanwa_data.com_data.pressure_4);
    CONSOLE_WRITE("  Temperatures: [0] %.2f, [1] %.2f", tanwa_data.com_data.temperature_1,
                  tanwa_data.com_data.temperature_2);
    CONSOLE_WRITE("  Igniters: [0] %d, [1] %d", tanwa_data.com_data.igniter_cont_1, tanwa_data.com_data.igniter_cont_2);
    CONSOLE_WRITE("HX OXI: ");
    CONSOLE_WRITE("  Temp: %d", tanwa_data.can_hx_oxidizer_status.temperature);
    CONSOLE_WRITE("  Weight: %.2f, Weight Raw: %d", tanwa_data.can_hx_oxidizer_data.weight,
                  tanwa_data.can_hx_oxidizer_data.weight_raw);
    CONSOLE_WRITE("HX RCK: ");
    CONSOLE_WRITE("  Temp: %d", tanwa_data.can_hx_rocket_status.temperature);
    CONSOLE_WRITE("  Weight: %.2f, Weight Raw: %d", tanwa_data.can_hx_rocket_data.weight,
                  tanwa_data.can_hx_rocket_data.weight_raw);
    CONSOLE_WRITE("FAC: ");
    CONSOLE_WRITE("  Motor: [0] %d, [1] %d", tanwa_data.can_fac_status.motor_state_1, tanwa_data.can_fac_status.motor_state_2);
    CONSOLE_WRITE("  Limit: [0] %d, [1] %d, [2] %d, [3] %d", tanwa_data.can_fac_status.limit_switch_1, tanwa_data.can_fac_status.limit_switch_2, 
                  tanwa_data.can_fac_status.limit_switch_3, tanwa_data.can_fac_status.limit_switch_4);
    CONSOLE_WRITE("  Servo: [0] %d, [1] %d", tanwa_data.can_fac_status.servo_state_1, tanwa_data.can_fac_status.servo_state_2);
    CONSOLE_WRITE("FLC: ");
    CONSOLE_WRITE("  Temperatures: [0] %d, [1] %d, [2] %d, [3] %d", tanwa_data.can_flc_data.temperature_1,
                  tanwa_data.can_flc_data.temperature_2, tanwa_data.can_flc_data.temperature_3,
                  tanwa_data.can_flc_data.temperature_4);
    CONSOLE_WRITE("TERMO: ");
    CONSOLE_WRITE("  Pressure: %.2f, Temperature: %d", tanwa_data.can_termo_data.pressure,
                  tanwa_data.can_termo_data.temperature);
    return 0;
}

static int get_com_board_data(int argc, char **argv) {
    com_data_t com_data = {0};
    com_data = tanwa_data_read_com_data();
    CONSOLE_WRITE("COM Data:");
    CONSOLE_WRITE("Battery Voltage: %.2f", com_data.vbat);
    CONSOLE_WRITE("Pressure 1: %.2f", com_data.pressure_1);
    CONSOLE_WRITE("Pressure 2: %.2f", com_data.pressure_2);
    CONSOLE_WRITE("Pressure 3: %.2f", com_data.pressure_3);
    CONSOLE_WRITE("Pressure 4: %.2f", com_data.pressure_4);
    CONSOLE_WRITE("Temperature 1: %.2f", com_data.temperature_1);
    CONSOLE_WRITE("Temperature 2: %.2f", com_data.temperature_2);
    return 0;
}

static int get_hx_oxi_data(int argc, char **argv) {
    can_hx_oxidizer_data_t hx_oxi_data = tanwa_data_read_can_hx_oxidizer_data();
    CONSOLE_WRITE("HX OXI Data:");
    CONSOLE_WRITE("Weight: %.2f", hx_oxi_data.weight);
    CONSOLE_WRITE("Weight Raw: %d", hx_oxi_data.weight_raw);
    return 0;
}

static int get_hx_rck_data(int argc, char **argv) {
    can_hx_rocket_data_t hx_rck_data = tanwa_data_read_can_hx_rocket_data();
    CONSOLE_WRITE("HX RCK Data:");
    CONSOLE_WRITE("Weight: %.2f", hx_rck_data.weight);
    CONSOLE_WRITE("Weight Raw: %d", hx_rck_data.weight_raw);
    return 0;
}

static int get_flc_data(int argc, char **argv) {
    can_flc_data_t flc_data = tanwa_data_read_can_flc_data();
    CONSOLE_WRITE("FLC Data:");
    CONSOLE_WRITE("Temperature 1: %d", flc_data.temperature_1);
    CONSOLE_WRITE("Temperature 2: %d", flc_data.temperature_2);
    CONSOLE_WRITE("Temperature 3: %d", flc_data.temperature_3);
    CONSOLE_WRITE("Temperature 4: %d", flc_data.temperature_4);
    return 0;
}

static int get_termo_data(int argc, char **argv) {
    can_termo_data_t termo_data = tanwa_data_read_can_termo_data();
    CONSOLE_WRITE("TERMO Data:");
    CONSOLE_WRITE("Pressure: %.2f", termo_data.pressure);
    CONSOLE_WRITE("Temperature: %d", termo_data.temperature);
    return 0;
}

static int get_valve_pressure_data(int argc, char **argv) {
    now_main_valve_pressure_data_t valve_pressure_data = tanwa_data_read_now_main_valve_pressure_data();
    CONSOLE_WRITE("Valve Pressure Data:");
    CONSOLE_WRITE("Pressure 1: %.2f", valve_pressure_data.pressure_1);
    CONSOLE_WRITE("Pressure 2: %.2f", valve_pressure_data.pressure_2);
    return 0;
}

static int get_valve_temperature_data(int argc, char **argv) {
    now_main_valve_temperature_data_t valve_temperature_data = tanwa_data_read_now_main_valve_temperature_data();
    CONSOLE_WRITE("Valve Temperature Data:");
    CONSOLE_WRITE("Temperature 1: %.2f", valve_temperature_data.temperature_1);
    CONSOLE_WRITE("Temperature 2: %.2f", valve_temperature_data.temperature_2);
    return 0;
}

static esp_console_cmd_t cmd[] = {
    // system commands
    {"reset-dev", "restart device", NULL, reset_device, NULL},
    {"reset-rck", "reset hx rck", NULL, reset_hx_rck, NULL},
    {"log-enable", "enable logs", NULL, enable_log, NULL},
    {"log-disable", "disable logs", NULL, disable_log, NULL},
    // state machine commands
    {"state-change", "change state", NULL, force_change_state, NULL},
    {"state-change-prev", "change state to previous", NULL, change_to_previous_state, NULL},
    {"state-change-force", "force change state", NULL, force_change_state, NULL},
    {"state-get", "get current state", NULL, get_state, NULL},
    // measurements commands
    {"temp-read", "read temperature", NULL, read_temperature, NULL},
    {"pressure-read", "read pressure", NULL, read_pressure, NULL},
    {"vbat-read", "read vbat voltage", NULL, read_vbat, NULL},
    // solenoid valve commands
    {"valve-open", "open solenoid valve", "f|d|a|fl|o|v", open_solenoid, NULL},
    {"valve-close", "close solenoid valve", "f|d|a|fl|o|v", close_solenoid, NULL},
    {"valve-toggle", "toggle solenoid valve", "f|d|a|fl|o|v", toggle_solenoid, NULL},
    // igniter commands
    {"igniter-arm", "arm igniter", "1|2|a", arm_igniter, NULL},
    {"igniter-disarm", "disarm igniter", "1|2|a", disarm_igniter, NULL},
    {"igniter-fire", "fire igniter", "1|2|a", fire_igniter, NULL},
    {"igniter-reset", "reset igniter", "1|2|a", reset_igniter, NULL},
    {"igniter-continuity", "check igniters continuity", NULL, check_igniter_continuity, NULL},
    // hx rck commands
    {"rck-tare", "tare hx rck", NULL, hx_rck_tare, NULL},
    {"rck-calibrate", "calibrate hx rck", NULL, hx_rck_calibrate, NULL},
    {"rck-set-calib", "set the calibration factor", "factor", hx_rck_set_calib_factor, NULL},
    {"rck-set-offset", "set the offset", "offset", hx_rck_set_offset, NULL},
    // hx oxi commands
    {"oxi-tare", "tare hx oxi", NULL, hx_oxi_tare, NULL},
    {"oxi-calibrate", "calibrate hx oxi", NULL, hx_oxi_calibrate, NULL},
    {"oxi-set-calib", "set the calibration factor", "factor", hx_oxi_set_calib_factor, NULL},
    {"oxi-set-offset", "set the offset", "offset", hx_oxi_set_offset, NULL},
    // quick disconnect commands
    {"qd-pull", "pull quick disconnect", NULL, qd_pull, NULL},
    {"qd-stop", "stop quick disconnect", NULL, qd_stop, NULL},
    {"qd-push", "push quick disconnect", NULL, qd_push, NULL},
    // liquid servo commands
    {"fac-servo-open", "open FAC servo", NULL, fac_servo_open, NULL},
    {"fac-servo-close", "close FAC servo", NULL, fac_servo_close, NULL},
    {"fac-servo-open-angle", "open FAC servo to angle", "angle", fac_servo_open_angle, NULL},
    // termo commands
    {"termo-heat-start", "start termo heating", NULL, termo_heat_start, NULL},
    {"termo-heat-stop", "stop termo heating", NULL, termo_heat_stop, NULL},
    {"termo-set-max", "set termo max pressure", "max_pressure", termo_set_max_pressure, NULL},
    {"termo-set-min", "set termo min pressure", "min_pressure", termo_set_min_pressure, NULL},
    // measument task commands
    {"measure-period", "change measurement period", "period", change_measure_period, NULL},
    // tanwa data commands
    {"tanwa-data", "get tanwa data", NULL, get_tanwa_data, NULL},
    {"com-data", "get com data", NULL, get_com_board_data, NULL},
    {"oxi-data", "get hx oxi data", NULL, get_hx_oxi_data, NULL},
    {"rck-data", "get hx rck data", NULL, get_hx_rck_data, NULL},
    {"flc-data", "get flc data", NULL, get_flc_data, NULL},
    {"termo-data", "get termo data", NULL, get_termo_data, NULL},
    {"pressure-valve-data", "get valve pressure data", NULL, get_valve_pressure_data, NULL},
    {"temperature-valve-data", "get valve temperature data", NULL, get_valve_temperature_data, NULL},
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