/**
 * @file valve_control.c
 * @author Michal Kos
 * @brief Valve control interface implementation for servo and solenoid valves
 * @version 0.1
 * @date 2022-08-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "valve_control.h"

#include "esp_log.h"

#include "servo_control.h"


static const char* TAG = "VALVE_CONTROL";

uint16_t valve_init_servo(adc_oneshot_unit_handle_t* adc_handle, adc_cali_handle_t* adc_cali_handle, bool adc_cali_enable, Valve_Servo_t* ValveServoInstance) {
  ValveServoInstance->config.pot_adc_handle = adc_handle;
  ValveServoInstance->config.pot_adc_cali_handle = adc_cali_handle;
  ValveServoInstance->config.pot_cali_enable = adc_cali_enable;

  // init servo
  servo_init(&ValveServoInstance->servo);

  // init potentiometer
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_POT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(
      *(ValveServoInstance->config.pot_adc_handle),
      ValveServoInstance->config.pot_adc_channel, &config));

  // init limit switches
  gpio_reset_pin(ValveServoInstance->config.limit_switch_pin_1);
  gpio_set_direction(ValveServoInstance->config.limit_switch_pin_1,
                     GPIO_MODE_INPUT);
  gpio_reset_pin(ValveServoInstance->config.limit_switch_pin_2);
  gpio_set_direction(ValveServoInstance->config.limit_switch_pin_2,
                     GPIO_MODE_INPUT);

  ValveServoInstance->Initialized = 1;

  if(ValveServoInstance->config.open_angle == 0 && ValveServoInstance->config.close_angle == 0){
    ESP_LOGE(TAG, "Valve angles not set");
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}

uint16_t read_angle(Valve_Servo_t* valve_servo_ptr) {
  ESP_ERROR_CHECK(adc_oneshot_read(*(valve_servo_ptr->config.pot_adc_handle),
                                   valve_servo_ptr->config.pot_adc_channel,
                                   (int*)&valve_servo_ptr->pot_adc_raw));

  // ESP_LOGI(TAG, "SERVO POT Raw Data: %d", ADC_UNIT_1 + 1,
  //          valve_servo_ptr->config.pot_adc_channel,
  //          valve_servo_ptr->pot_adc_raw);

  if (valve_servo_ptr->config.pot_cali_enable) {
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(
        *(valve_servo_ptr->config.pot_adc_cali_handle),
        valve_servo_ptr->pot_adc_raw, (int*)&valve_servo_ptr->pot_voltage));
    // ESP_LOGI(TAG, "SERVO POT Cali Voltage: %d mV", ADC_UNIT_1 + 1,
    //          valve_servo_ptr->config.pot_adc_channel,
    //          valve_servo_ptr->pot_voltage);
  }

  valve_servo_ptr->pot_angle_value = valve_servo_ptr->pot_adc_raw;

  return RET_SUCCESS;
}



uint16_t read_limits(Valve_Servo_t* valve_servo_ptr) {
  valve_servo_ptr->limit_state_1 =
      gpio_get_level(valve_servo_ptr->config.limit_switch_pin_1);
  valve_servo_ptr->limit_state_2 =
      gpio_get_level(valve_servo_ptr->config.limit_switch_pin_2);
  return RET_SUCCESS;
}

uint16_t check_servo_limits(Valve_Servo_t* valve_servo_ptr) {
  read_limits(valve_servo_ptr);
  read_angle(valve_servo_ptr);

  if (valve_servo_ptr->limit_state_2 ||
      ((int16_t)valve_servo_ptr->pot_angle_value >= (POT_VALVE_OPEN_VALUE) &&
       valve_servo_ptr->pot_angle_value <= (POT_VALVE_OPEN_VALUE + 20))) {
    ESP_LOGI(TAG, "Valve open");
    valve_servo_ptr->valve_state = OPEN_STATE;
  } else if (valve_servo_ptr->limit_state_1 ||
             (valve_servo_ptr->pot_angle_value >=
                  (POT_VALVE_CLOSE_VALUE - 20) &&
              valve_servo_ptr->pot_angle_value <=
                  (POT_VALVE_CLOSE_VALUE + 20))) {
    ESP_LOGI(TAG, "Valve close");
    valve_servo_ptr->valve_state = CLOSE_STATE;
  }
  return RET_SUCCESS;
}

// Calibration for servo version
uint16_t calibrate_angles(Valve_Servo_t* valve_servo_ptr) {
  return RET_SUCCESS;
}

// Limit switches callback functions
uint16_t limit_callback(Valve_Servo_t* valve_servo_ptr) { return RET_SUCCESS; }

uint16_t valve_get_state_servo(Valve_Servo_t* valve_servo_ptr) {
  return valve_servo_ptr->valve_state;
}

uint16_t valve_move_angle_servo(Valve_Servo_t* valve_servo_ptr,
                                uint16_t angle) {
  move_servo(&valve_servo_ptr->servo, (float)angle / 90.0 * (valve_servo_ptr->config.open_angle - valve_servo_ptr->config.close_angle) + valve_servo_ptr->config.close_angle);
  return RET_SUCCESS;
}

uint16_t valve_open_servo(Valve_Servo_t* valve_servo_ptr) {
  ESP_LOGI(TAG, "Valve servo open");
  move_servo(&valve_servo_ptr->servo, valve_servo_ptr->config.open_angle);
  valve_servo_ptr->valve_state = OPEN_STATE;
  return RET_SUCCESS;
}

uint16_t valve_close_servo(Valve_Servo_t* valve_servo_ptr) {
  ESP_LOGI(TAG, "Valve servo close");
  move_servo(&valve_servo_ptr->servo, valve_servo_ptr->config.close_angle);
  valve_servo_ptr->valve_state = CLOSE_STATE;
  return RET_SUCCESS;
}

uint16_t valve_time_open_servo(Valve_Servo_t* valve_servo_ptr,
                               const uint16_t open_time) {
  ESP_LOGI(TAG, "Valve servo time open");
  valve_open_servo(valve_servo_ptr);
  vTaskDelay(pdMS_TO_TICKS(open_time));
  valve_close_servo(valve_servo_ptr);
  return RET_SUCCESS;
}