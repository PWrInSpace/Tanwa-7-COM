#include "settings_mem.h"

static Settings settings;

// Private declarations:


static nvs_handle_t settings_handle;


// Function definitions:

void settings_init_default() {
    settings.countdownTime = -50000;
    settings.ignitTime = -40000;
    settings.lora_transmit_ms = 1800;
    settings.loraFreq_KHz = 915000;
    settings.flash_on = 0;
    settings.fuel_full_open_time_ms = 1500;
    settings.oxidizer_full_open_time_ms = 1000;
    settings.fuel_valve_initial_angle = 15;
    settings.oxidizer_valve_initial_angle = 15;
    settings.fuel_open_time_ms = 500;

    settings_save_all();
}

esp_err_t settings_init() {

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err != ESP_OK) {
        return err;
    }

    if (settings_read_all() != ESP_OK) {
        return false;
    }

    if (settings.countdownTime == 0) {
        settings_init_default();
    }

    return settings_read_all();
}

esp_err_t settings_save_all() {

    esp_err_t err = settings_save(SETTINGS_LORA_FREQ_KHZ, settings.loraFreq_KHz);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_LORA_TRANSMIT_MS, settings.lora_transmit_ms);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_COUNTDOWN_TIME, settings.countdownTime);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_IGNIT_TIME, settings.ignitTime);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    // err = settings_save(SETTINGS_BUZZER_ON, settings.buzzer_on);
    // if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(SETTINGS_FLASH_ON, settings.flash_on);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(FUEL_VALVE_INITIAL_ANGLE, settings.fuel_valve_initial_angle);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(OXIDIZER_VALVE_INITIAL_ANGLE, settings.oxidizer_valve_initial_angle);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(FUEL_OPEN_TIME_MS, settings.fuel_open_time_ms);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(OXIDIZER_FULL_OPEN_TIME_MS, settings.oxidizer_full_open_time_ms);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    err = settings_save(FUEL_FULL_OPEN_TIME_MS, settings.fuel_full_open_time_ms);
    if (err) ESP_LOGE(SETTINGS_TAG, "%s\n", esp_err_to_name(err));

    return ESP_OK;
}

esp_err_t settings_read_all() {

    settings.loraFreq_KHz   = settings_read(SETTINGS_LORA_FREQ_KHZ);
    settings.lora_transmit_ms    = settings_read(SETTINGS_LORA_TRANSMIT_MS);
    settings.countdownTime  = settings_read(SETTINGS_COUNTDOWN_TIME);
    settings.ignitTime      = settings_read(SETTINGS_IGNIT_TIME);
    // settings.buzzer_on = settings_read(SETTINGS_BUZZER_ON);
    settings.flash_on = settings_read(SETTINGS_FLASH_ON);
    settings.fuel_valve_initial_angle = settings_read(FUEL_VALVE_INITIAL_ANGLE);
    settings.oxidizer_valve_initial_angle = settings_read(OXIDIZER_VALVE_INITIAL_ANGLE);
    settings.fuel_open_time_ms = settings_read(FUEL_OPEN_TIME_MS);
    settings.oxidizer_full_open_time_ms = settings_read(OXIDIZER_FULL_OPEN_TIME_MS);
    settings.fuel_full_open_time_ms = settings_read(FUEL_FULL_OPEN_TIME_MS);

    return ESP_OK;
}

int32_t settings_read(char* setting_name) {

    nvs_open(SETTINGS_NAMESPACE, NVS_READONLY, &settings_handle);

    int32_t requestedVal = 0;
    nvs_get_i32(settings_handle, setting_name, &requestedVal);

    nvs_close(settings_handle);
    return requestedVal;
}

Settings settings_get_all(void) {
    return settings;
}

esp_err_t settings_save(char* setting_name, int32_t newVal) {

    // Check if the new value is different:
    int32_t oldVal = settings_read(setting_name);
    if (oldVal == newVal) return ESP_OK;

    // Save new value:
    esp_err_t err = nvs_open(SETTINGS_NAMESPACE, NVS_READWRITE, &settings_handle);

    err = nvs_set_i32(settings_handle, setting_name, newVal);

    // Commit changes to flash:
    err = nvs_commit(settings_handle);
    nvs_close(settings_handle);

    return err;
}