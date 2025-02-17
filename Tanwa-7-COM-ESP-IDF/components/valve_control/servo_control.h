/**
 * @file servo_control.h
 * @author Michal Kos
 * @brief Servo control interface
 * @version 0.1
 * @date 2022-08-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SERVO_CONTROL_HH
#define SERVO_CONTROL_HH

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "driver/mcpwm_prelude.h"

/**************************  CONFIG INCLUDES  *********************************/

/**************************  DEFINES  *****************************************/

// defines to be overwritten by config
#define SERVO_MIN_PULSEWIDTH_US 500   // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_ANGLE 0             // Minimum angle
#define SERVO_MAX_ANGLE 180           // Maximum angle
#define SERVO_FREQUENCY_HZ 1000000  // frequency of the pwm signal for the servo
#define SERVO_TIMEBASE_PERIOD 20000  // 20000 ticks, 20ms

#define RET_SUCCESS 0x00
#define RET_FAILURE 0x01

/**************************  INIT *********************************************/

#define SERVO_INIT(X)                                              \
  {                                                                \
    .pwm_pin = X, .timer = NULL, .oper = NULL, .comparator = NULL, \
    .generator = NULL, .open_angle = 0, .close_angle = 0}

/**************************  PUBLIC VARIABLES  ********************************/

/**************************  CODE *********************************************/

typedef struct Servo {
    uint32_t pwm_pin; /**< GPIO connects to the PWM signal line */
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;
    uint16_t close_angle;
    uint16_t open_angle;
} Servo_t;

uint16_t servo_init(Servo_t *servo_ptr);  // ADD STATUS REPORTING

uint16_t move_servo(Servo_t *servo_ptr, uint8_t angle);  // ADD STATUS REPORTING

#endif  // SERVO_CONTROL_HH