// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "mcu_twai_config.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TAG "MCU_TWAI_CONFIG"

//Example Configurations
#define NO_OF_MSGS              100
#define NO_OF_ITERS             3
#define TX_GPIO_NUM             CONFIG_CAN_TX
#define RX_GPIO_NUM             CONFIG_CAN_RX
#define TX_TASK_PRIO            8       //Sending task priority
#define RX_TASK_PRIO            9       //Receiving task priority
#define CTRL_TSK_PRIO           10      //Control task priority
#define MSG_ID                  0x555   //11 bit standard format ID
#define EXAMPLE_TAG             "TWAI Self Test"

mcu_twai_config_t mcu_twai_config = {
    .tx_gpio_num = CONFIG_CAN_RX,
    .rx_gpio_num = CONFIG_CAN_TX,
    .mode = TWAI_MODE_NORMAL,
    .g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL),
    .t_config = TWAI_TIMING_CONFIG_500KBITS(),
    .f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(),
};

static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
//Filter all other IDs except MSG_ID
static const twai_filter_config_t f_config = {.acceptance_code = (MSG_ID << 21),
                                              .acceptance_mask = ~(TWAI_STD_ID_MASK << 21),
                                              .single_filter = true
                                             };
//Set to NO_ACK mode due to self testing with single module
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);

static SemaphoreHandle_t tx_sem;
static SemaphoreHandle_t rx_sem;
static SemaphoreHandle_t ctrl_sem;
static SemaphoreHandle_t done_sem;

static void twai_transmit_task(void *arg) {
  twai_message_t tx_msg = {.data_length_code = 1, .identifier = MSG_ID, .self = 1};
  for (int iter = 0; iter < NO_OF_ITERS; iter++) {
      xSemaphoreTake(tx_sem, portMAX_DELAY);
      for (int i = 0; i < NO_OF_MSGS; i++) {
          //Transmit messages using self reception request
          tx_msg.data[0] = i;
          ESP_ERROR_CHECK(twai_transmit(&tx_msg, portMAX_DELAY));
          vTaskDelay(pdMS_TO_TICKS(10));
      }
  }
  vTaskDelete(NULL);
}

static void twai_receive_task(void *arg) {
  twai_message_t rx_message;

  for (int iter = 0; iter < NO_OF_ITERS; iter++) {
      xSemaphoreTake(rx_sem, portMAX_DELAY);
      for (int i = 0; i < NO_OF_MSGS; i++) {
          //Receive message and print message data
          ESP_ERROR_CHECK(twai_receive(&rx_message, portMAX_DELAY));
          ESP_LOGI(EXAMPLE_TAG, "Msg received - Data = %d", rx_message.data[0]);
      }
      //Indicate to control task all messages received for this iteration
      xSemaphoreGive(ctrl_sem);
  }
  vTaskDelete(NULL);
}

static void twai_control_task(void *arg) {
  xSemaphoreTake(ctrl_sem, portMAX_DELAY);
  for (int iter = 0; iter < NO_OF_ITERS; iter++) {
      //Start TWAI Driver for this iteration
      ESP_ERROR_CHECK(twai_start());
      ESP_LOGI(EXAMPLE_TAG, "Driver started");

      //Trigger TX and RX tasks to start transmitting/receiving
      xSemaphoreGive(rx_sem);
      xSemaphoreGive(tx_sem);
      xSemaphoreTake(ctrl_sem, portMAX_DELAY);    //Wait for TX and RX tasks to finish iteration

      ESP_ERROR_CHECK(twai_stop());               //Stop the TWAI Driver
      ESP_LOGI(EXAMPLE_TAG, "Driver stopped");
      vTaskDelay(pdMS_TO_TICKS(100));             //Delay then start next iteration
  }
  xSemaphoreGive(done_sem);
  vTaskDelete(NULL);
}

esp_err_t mcu_twai_init() {
  esp_err_t err;
  err = twai_driver_install(&(mcu_twai_config.g_config), 
                            &(mcu_twai_config.t_config),
                            &(mcu_twai_config.f_config));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "TWAI driver install error");
    return err;
  }
  err = twai_start();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "TWAI start error");
    return err;
  }
  return ESP_OK;
}

twai_message_t mcu_twai_compose_self_test_message(uint32_t id,
                                                  uint8_t data_length_code,
                                                  uint8_t *data) {
  twai_message_t msg = { 
    .identifier = id, 
    .data_length_code = data_length_code, 
    .self = 1
  };
  for (int i = 0; i < data_length_code; i++) {
    msg.data[i] = data[i];
  }
  return msg;
}

void mcu_twai_self_test() {
  //Create tasks and synchronization primitives
  tx_sem = xSemaphoreCreateBinary();
  rx_sem = xSemaphoreCreateBinary();
  ctrl_sem = xSemaphoreCreateBinary();
  done_sem = xSemaphoreCreateBinary();

  xTaskCreatePinnedToCore(twai_control_task, "TWAI_ctrl", 4096, NULL, CTRL_TSK_PRIO, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(twai_transmit_task, "TWAI_tx", 4096, NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);

  //Install TWAI driver
  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_LOGI(EXAMPLE_TAG, "Driver installed");

  //Start control task
  xSemaphoreGive(ctrl_sem);
  //Wait for all iterations and tasks to complete running
  xSemaphoreTake(done_sem, portMAX_DELAY);

  //Uninstall TWAI driver
  ESP_ERROR_CHECK(twai_driver_uninstall());
  ESP_LOGI(EXAMPLE_TAG, "Driver uninstalled");

  //Cleanup
  vSemaphoreDelete(tx_sem);
  vSemaphoreDelete(rx_sem);
  vSemaphoreDelete(ctrl_sem);
  vQueueDelete(done_sem);
}
