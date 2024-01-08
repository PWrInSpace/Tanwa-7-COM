// Copyright 2023 PWr in Space, Krzysztof Gliwiński

#include "app_init_task.h"

#define TAG "APP_INIT_TASK"

#define I2C_MASTER_FREQ_HZ 400000  // TODO(Glibus): sdkconig.h should have it

static adc_oneshot_unit_handle_t adc1_handle;

static sdmmc_card_t sdmmc_card;

static sd_card_t sd_card = SD_CARD_DEFAULT_CONFIG(sdmmc_card);

static sd_card_config_t sd_card_conf = SD_CARD_CONFIG_DEFAULT_CONFIG();

static uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN,
                       TUD_OPT_HIGH_SPEED ? 512 : 64),
};

static const tusb_desc_device_t descriptor_config = {
    .bLength = sizeof(descriptor_config),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x303A,  // This is Espressif VID. This needs to be changed
                         // according to Users / Customers
    .idProduct = 0x4002,
    .bcdDevice = 0x100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01};

static char const* string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "TinyUSB",                   // 1: Manufacturer
    "TinyUSB Device",            // 2: Product
    "123456",                    // 3: Serials
    "Example MSC",               // 4. MSC
};

static const wl_handle_t wl = WL_INVALID_HANDLE;

static const tinyusb_msc_spiflash_config_t config_spi = {.wl_handle = wl};

static const tinyusb_msc_sdmmc_config_t config_sdmmc = {
    .card = &sdmmc_card,
};

static usb_msc_config_t usb_msc =
    USB_MSC_CONFIG_DEFAULT_CONFIG(descriptor_config, string_desc_arr,
                                  desc_configuration, config_sdmmc, config_spi);

static ROSALIA_devices_t devices_config = {
    .spi = MCU_SPI_DEFAULT_CONFIG(),
    .i2c = MCU_I2C_DEFAULT_CONFIG(),
    .twai = MCU_TWAI_DEFAULT_CONFIG(),
    .voltage_measure = MCU_VOLTAGE_MEASURE_DEFAULT_CONFIG(),
    .lora = MCU_LORA_DEFAULT_CONFIG(),
    .oled_display = MCU_SSD1306_DEFAULT_CONFIG(),
    .rgb_led = MCU_RGB_LED_DRIVER_DEFAULT_CONFIG(),
    .buzzer = MCU_BUZZER_DRIVER_DEFAULT_CONFIG(),
    .memory =
        {
            .sd_card = &sd_card,
            .sd_card_config = &sd_card_conf,
            .usb_msc_config = &usb_msc,
        },
};

void app_init_task(void* pvParameters) {
  ESP_LOGI(TAG, "Initializing devices");

  devices_config.voltage_measure.oneshot_unit_handle = &adc1_handle;

  esp_err_t ret = devices_init(&devices_config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Devices initialization failed");
    vTaskDelete(NULL);
  }

  for (int i = 0; i < 3; i++) {
    // ESP_LOGI(TAG, "%d", &devices_config->voltage_measure.adc_chan[i]);
  }

  xTaskCreatePinnedToCore(user_interface_task, "user_interface_task", 4096,
                          (void*)&devices_config, 1, NULL, 1);
  // xTaskCreatePinnedToCore(slave_com_task, "slave_com_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
  xTaskCreatePinnedToCore(lora_task, "lora_task", 4096, (void*)&devices_config,
                          1, NULL, 1);
  // xTaskCreatePinnedToCore(memory_task, "mcu_memory_task", 4096,
  //                         (void*)&devices_config, 1, NULL, 1);
}
