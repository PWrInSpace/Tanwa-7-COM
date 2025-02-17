#ifndef VALVE_CONTROL_H
#define VALVE_CONTROL_H

#include <stdint.h>

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

#include "servo_control.h"


#define POT_VALVE_OPEN_VALUE 0U
#define POT_VALVE_CLOSE_VALUE 860U

#define CLOSE_STATE 0U
#define OPEN_STATE 1U
#define BETWEEN_STATE 2U

#define ADC_ATTEN_POT ADC_ATTEN_DB_11

typedef struct Valve_Servo_config {
    // limit switches
    const uint8_t limit_switch_pin_1;  // Valve closed
    const uint8_t limit_switch_pin_2;  // Valve opened
    uint32_t limit_time_1;
    uint32_t limit_time_2;
    // pot adc settings
    adc_channel_t pot_adc_channel;
    adc_oneshot_unit_handle_t* pot_adc_handle;
    adc_cali_handle_t* pot_adc_cali_handle;
    bool pot_cali_enable;
    // valve config
    uint8_t close_angle;
    uint8_t open_angle;
} Valve_Servo_config_t;

typedef struct Valve_Servo {
    uint8_t Initialized;
    // servo object and config
    Servo_t servo;
    Valve_Servo_config_t config;
    // set and pot angles
    uint16_t angle_value;
    uint16_t pot_angle_value;
    // pot reads
    uint32_t pot_adc_raw;
    uint32_t pot_voltage;
    // limit switches states
    uint8_t limit_state_1;
    uint8_t limit_state_2;
    // overall valve state
    uint8_t valve_state;
} Valve_Servo_t;

// Read valve angle from potentiometer
uint16_t read_angle(Valve_Servo_t* valve_servo);

// Read state of the limit switches
uint16_t read_limits(Valve_Servo_t* valve_servo_ptr);

// Read valve angle from pot and read limits to determine the valve state
uint16_t check_servo_limits(Valve_Servo_t* valve_servo_ptr);

// Limit switches callback functions
uint16_t limit_callback(Valve_Servo_t* valve_servo);

// Calibration for servo version
uint16_t calibrate_angles(Valve_Servo_t* valve_servo);

uint16_t valve_init_servo(adc_oneshot_unit_handle_t* adc_handle, adc_cali_handle_t* adc_cali_handle, bool adc_cali_enable, Valve_Servo_t *ValveServoInstance);

uint16_t valve_get_state_servo(Valve_Servo_t* valve_servo);

uint16_t valve_move_angle_servo(Valve_Servo_t* valve_servo_ptr, uint16_t angle);

uint16_t valve_open_servo(Valve_Servo_t* valve_servo);

uint16_t valve_close_servo(Valve_Servo_t* valve_servo);

uint16_t valve_time_open_servo(Valve_Servo_t* valve_servo,
                               const uint16_t open_time);


#endif // VALVE_CONTROL_H