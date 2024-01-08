// Copyright 2023 PWrInSpace, Krzysztof Gliwiński

#include "mcu_memory_config.h"
#include "sdkconfig.h"
#include "unity.h"

static const sdmmc_card_t sdmmc_card;

static const spi_host_device_t host = SPI2_HOST;

static sd_card_t sd_card = {
    .card = &sdmmc_card,
    .spi_host = host,
    .mount_point = SDCARD_MOUNT_POINT,
    .cs_pin = CONFIG_SD_CS,
    .card_detect_pin = CONFIG_SD_CD,
    .mounted = false,
};

const sd_card_config_t sd_card_conf = {
    .spi_host = host,
    .mount_point = SDCARD_MOUNT_POINT,
    .cs_pin = CONFIG_SD_CS,
    .cd_pin = CONFIG_SD_CD,
};

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

static char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
    "TinyUSB",                   // 1: Manufacturer
    "TinyUSB Device",            // 2: Product
    "123456",                    // 3: Serials
    "Example MSC",               // 4. MSC
};

static const tinyusb_config_t tusb_config = {
    .device_descriptor = &descriptor_config,
    .string_descriptor = string_desc_arr,
    .string_descriptor_count =
        sizeof(string_desc_arr) / sizeof(string_desc_arr[0]),
    .external_phy = false,
    .configuration_descriptor = desc_configuration,
};

static const wl_handle_t wl = WL_INVALID_HANDLE;

static const tinyusb_msc_spiflash_config_t config_spi = {.wl_handle = wl};

static const tinyusb_msc_sdmmc_config_t config_sdmmc = {
    .card = &sdmmc_card,
};

static usb_msc_config_t usb_msc = {
    .desc_device = descriptor_config,
    .tusb_cfg = tusb_config,
    .sd_config = config_sdmmc,
    .spiflash_config = config_spi,
    .mount_path = BASE_PATH,
    .current_storage_type = USB_MSC_INIT_STORAGE_NONE,
};

static mcu_memory_config_t memory_config = {.sd_card = &sd_card,
                                            .sd_card_config = &sd_card_conf,
                                            .usb_msc_config = &usb_msc};

static mcu_spi_config_t spi_config = MCU_SPI_DEFAULT_CONFIG();

TEST_CASE("MCU memory init test", "[MCU_MEMORY]") {
  TEST_ASSERT_EQUAL(ESP_OK, spi_init(&spi_config));
  memory_config_mount_spi_config(&memory_config, &spi_config);
  esp_err_t ret = memory_init(&memory_config);
  TEST_ASSERT_EQUAL(ESP_OK, ret);
}

// TEST_CASE("MCU memory usb otg spi flash activation test", "[MCU_MEMORY]") {
//   esp_err_t ret = memory_usb_msc_activate(&memory_config,
//                                           USB_MSC_INIT_STORAGE_TYPE_SPI_FLASH);
//   TEST_ASSERT_EQUAL(ESP_OK, ret);
// }

// TEST_CASE("MCU memory usb otg spi flash deactivation test", "[MCU_MEMORY]") {
//   memory_usb_msc_deactivate(&memory_config);
//   TEST_ASSERT_EQUAL(usb_msc.current_storage_type, USB_MSC_INIT_STORAGE_NONE);
// }

TEST_CASE("MCU memory usb otg sd card activation test", "[MCU_MEMORY]") {
  esp_err_t ret = memory_usb_msc_activate(&memory_config,
                                          USB_MSC_INIT_STORAGE_TYPE_SDMMC);
  TEST_ASSERT_EQUAL(ESP_OK, ret);
  vTaskDelay(pdMS_TO_TICKS(2000));
}

TEST_CASE("MCU memory usb otg spi sd card deactivation test", "[MCU_MEMORY]") {
  memory_usb_msc_deactivate(&memory_config);
  TEST_ASSERT_EQUAL(usb_msc.current_storage_type, USB_MSC_INIT_STORAGE_NONE);
}

TEST_CASE("MCU memory Deinit", "[MEMORY_SPI]") {
  esp_err_t ret = memory_deinit(&memory_config);
  TEST_ASSERT_EQUAL(ESP_OK, ret);
}
