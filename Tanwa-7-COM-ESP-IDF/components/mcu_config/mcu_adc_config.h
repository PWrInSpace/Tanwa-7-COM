///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 28.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the configuration of the ADC peripheral for the MCU.
/// This can only be used for ADC1!
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_7_MCU_ADC_CONFIG_H_
#define PWRINSPACE_TANWA_7_MCU_ADC_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "soc/adc_channel.h"

#include "igniter_driver.h"

#define READ_ERROR_RETURN_VAL 0xFFFF
#define VOLTAGE_READ_ERROR_RETURN_VAL -1.0f
#define MAX_ADC_CHANNELS 8

#define MCU_VOLTAGE_MEASURE_DEFAULT_CONFIG()               \
  {                                                        \
    .adc_cal = {1.0f, 5.742f, 5.180f},                     \
    .adc_chan = {VBAT_CHANNEL, IGNITER_1_CHANNEL, IGNITER_2_CHANNEL}, \
    .adc_chan_num = MAX_CHANNEL_INDEX,                     \
    .oneshot_chan_cfg =                                    \
        {                                                  \
            .bitwidth = ADC_BITWIDTH_12,                   \
            .atten = ADC_ATTEN_DB_12,                      \
        },                                                 \
    .oneshot_unit_cfg = {                                  \
      .unit_id = ADC_UNIT_1,                               \
    }                                                      \
  }

typedef enum {
  VBAT_CHANNEL = ADC_CHANNEL_0,
  IGNITER_1_CHANNEL = ADC_CHANNEL_6,
  IGNITER_2_CHANNEL = ADC_CHANNEL_7
} adc_chan_cfg_t;

typedef enum {
  VBAT_CHANNEL_INDEX = 0,
  IGNITER_1_CHANNEL_INDEX,
  IGNITER_2_CHANNEL_INDEX,
  MAX_CHANNEL_INDEX
} adc_chan_index_cfg_t;

/*!
 * \brief Voltage measure struct
 * \param adc_cal - calibration value to be configured.
 *                 voltage = rawRead * adc_cal
 * \param adc_chan - specific channel of ADC
 * \param adc_chan_num - number of channels to be configured
 * \param oneshot_unit_cfg - config for ADC channel
 * \param oneshot_unit_handle - handle for ADC channel
 * \param oneshot_chan_cfg - config for ADC channel
 * \param oneshot_chan_handle - handle for ADC channel
 * \note adc_cal and adc_chan are arrays of size adc_chan_num.
 *      adc_cal[i] is calibration value for adc_chan[i]
 *     adc_chan[i] is channel number for adc_cal[i]
 * \note oneshot_unit_cfg and oneshot_chan_cfg are the same for all channels.
 */

typedef struct {
  float adc_cal[MAX_ADC_CHANNELS];
  uint8_t adc_chan[MAX_ADC_CHANNELS];
  uint8_t adc_chan_num;
  adc_oneshot_unit_init_cfg_t oneshot_unit_cfg;
  adc_oneshot_chan_cfg_t oneshot_chan_cfg;
  adc_oneshot_unit_handle_t* oneshot_unit_handle;
} voltage_measure_config_t;

/*!
  \brief Init for a voltage measure.
  \param v_mes - pointer to voltage_measure_config_t struct
  \param adc_chan - specific channel of ADC1
  \param adc_cal - calibration value to be configured.
                  voltage = rawRead * adc_cal
*/
esp_err_t voltage_measure_init(voltage_measure_config_t* v_mes);

/*!
 * \brief Read raw value from ADC
 * \param v_mes - pointer to voltage_measure_config_t struct
 * \param adc_chan - specific channel of ADC as specified in v_mes struct
 */
int voltage_measure_read_raw(voltage_measure_config_t* v_mes, uint8_t adc_chan);

/*!
 * \brief Read voltage from ADC
 * \param v_mes - pointer to voltage_measure_config_t struct
 * \param adc_chan - specific channel of ADC as specified in v_mes struct
 */
float voltage_measure_read_voltage(voltage_measure_config_t* v_mes,
                                   uint8_t adc_chan);

#endif /* PWRINSPACE_TANWA_7_MCU_ADC_CONFIG_H_ */