// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#include "nvs_flash.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

#include "storage.h"

static const char *TAG = "storage";

esp_err_t storage_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "Storage initialization failed.");
    return ESP_OK;
}