/**
 * @file servo_control.c
 * @author Michal Kos
 * @brief Servo control interface
 * @version 0.1
 * @date 2022-08-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "servo_control.h"

//#include "fac_config.h"

/**************************  PRIVATE INCLUDES  ********************************/

#include "esp_log.h"

/**************************  PRIVATE VARIABLES  *******************************/

static const char *TAG = "SERVO CONTROL";

/**************************  PRIVATE FUNCTIONS  *******************************/

static inline uint32_t angle_to_duty_us(uint8_t angle) {
  return (angle - SERVO_MIN_ANGLE) *
             (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) /
             (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) +
         SERVO_MIN_PULSEWIDTH_US;
}

/**************************  CODE *********************************************/

uint16_t servo_init(Servo_t *servo_ptr) {
  // create timer
  mcpwm_timer_config_t timer_config = {
      .group_id = 1,
      .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
      .resolution_hz = SERVO_FREQUENCY_HZ,
      .period_ticks = SERVO_TIMEBASE_PERIOD,
      .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
  };
  ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &servo_ptr->timer));
  // create operator
  mcpwm_operator_config_t operator_config = {
      .group_id = 1,  // operator must be in the same group to the timer
  };
  ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &servo_ptr->oper));
  // connect operator to timer
  ESP_ERROR_CHECK(
      mcpwm_operator_connect_timer(servo_ptr->oper, servo_ptr->timer));
  // create comparator
  mcpwm_comparator_config_t comparator_config = {
      .flags.update_cmp_on_tez = true,
  };
  ESP_ERROR_CHECK(mcpwm_new_comparator(servo_ptr->oper, &comparator_config,
                                       &servo_ptr->comparator));
  // create generator
  mcpwm_generator_config_t generator_config = {
      .gen_gpio_num = servo_ptr->pwm_pin,
  };
  ESP_ERROR_CHECK(mcpwm_new_generator(servo_ptr->oper, &generator_config,
                                      &servo_ptr->generator));
  // set the initial compare value, so that the servo will spin to the center
  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
      servo_ptr->comparator, angle_to_duty_us(servo_ptr->close_angle)));

  // Set generator action on timer and compare event
  // go high on counter empty
  ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_timer_event(
      servo_ptr->generator,
      MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                   MCPWM_TIMER_EVENT_EMPTY,
                                   MCPWM_GEN_ACTION_HIGH),
      MCPWM_GEN_TIMER_EVENT_ACTION_END()));
  // go low on compare threshold
  ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(
      servo_ptr->generator,
      MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                     servo_ptr->comparator,
                                     MCPWM_GEN_ACTION_LOW),
      MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
  // Enable and start timer
  ESP_ERROR_CHECK(mcpwm_timer_enable(servo_ptr->timer));
  ESP_ERROR_CHECK(
      mcpwm_timer_start_stop(servo_ptr->timer, MCPWM_TIMER_START_NO_STOP));
  return RET_SUCCESS;
}

uint16_t move_servo(Servo_t *servo_ptr, uint8_t angle) {
  ESP_LOGI(TAG, "Angle of rotation: %d", angle);

  if (mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(angle)) != ESP_OK) {
    ESP_LOGE(TAG, "Move servo FAILURE");
    return RET_FAILURE;
  }

  // Add delay, since it takes time for servo to rotate
  vTaskDelay(pdMS_TO_TICKS(10));

  return RET_SUCCESS;
}