///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 16.06.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the buzzer driver. The buzzer is controlled via GPIO pin
/// and through a transistor. The buzzer can be turned on and off.
///===-----------------------------------------------------------------------------------------===//
#ifndef PWRINSPACE_BUZZER_DRIVER_H_
#define PWRINSPACE_BUZZER_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"

typedef enum {
  BUZZER_OK = 0,
  BUZZER_FAIL,
  BUZZER_GPIO_ERR,
  BUZZER_INVALID_ARG,
  BUZZER_NULL_ARG,
} buzzer_driver_status_t;

typedef enum {
  BUZZER_STATE_OFF = 0,
  BUZZER_STATE_ON = 1,
} buzzer_driver_state_t;

typedef enum {
  BUZZER_POLARITY_ACTIVE_LOW = 0,
  BUZZER_POLARITY_ACTIVE_HIGH,
} buzzer_driver_polarity_t;

typedef bool (*buzzer_GPIO_set_level)(uint8_t _gpio_num, uint8_t _level);
typedef void (*buzzer_delay)(uint32_t _ms);

typedef struct {
  buzzer_GPIO_set_level _gpio_set_level;
  buzzer_delay _delay;
  uint8_t gpio_idx;
  buzzer_driver_polarity_t polarity;
  buzzer_driver_state_t state;
} buzzer_struct_t;

/*!
 * \brief Initialize buzzer driver
 * \param config buzzer configuration
 */
buzzer_driver_status_t buzzer_init(buzzer_struct_t *config);

/*!
  * \brief Set buzzer state
  * \param config buzzer configuration
  * \param state buzzer state
  */
buzzer_driver_status_t buzzer_set_state(buzzer_struct_t *config, buzzer_driver_state_t state);

/*!
 * @brief Get buzzer state
 * @param config buzzer configuration
 * @param state buzzer state pointer
 * @return esp_err_t 
 */
buzzer_driver_status_t buzzer_get_state(buzzer_struct_t *config, buzzer_driver_state_t *state);

/*!
 * \brief Toggle buzzer state
 * \param config buzzer configuration
 */
buzzer_driver_status_t buzzer_toggle_state(buzzer_struct_t *config);

#endif // PWRINSPACE_BUZZER_DRIVER_H_
