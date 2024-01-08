// Copyright 2023 PWrInSpace, Krzysztof Gliwiński

#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "soc/adc_channel.h"
/*!
  \file voltage_measure.h contains structs and functions to handle
        ESP32's ADC1 only!
*/

#define MCU_VOLTAGE_MEASURE_DEFAULT_CONFIG()               \
  {                                                        \
    .adc_cal = {1.0f, 5.742f, 5.180f},                     \
    .adc_chan = {CAN_CHANNEL, VBAT_CHANNEL, ADJV_CHANNEL}, \
    .adc_chan_num = MAX_CHANNEL_INDEX,                     \
    .oneshot_chan_cfg =                                    \
        {                                                  \
            .bitwidth = ADC_BITWIDTH_12,                   \
            .atten = ADC_ATTEN_DB_11,                      \
        },                                                 \
    .oneshot_unit_cfg = {                                  \
      .unit_id = ADC_UNIT_1,                               \
    }                                                      \
  }

#define READ_ERROR_RETURN_VAL 0xFFFF
#define VOLTAGE_READ_ERROR_RETURN_VAL -1.0f
#define MAX_ADC_CHANNELS 8

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
