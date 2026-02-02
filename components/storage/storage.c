// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#include "nvs_flash.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

#include "storage.h"

static const char *TAG = "storage";
static const char *NVS_NAMESPACE = "app";

esp_err_t storage_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "Storage initialization failed.");

    nvs_handle_t handle;
    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to initialize namespace");
    nvs_close(handle);

    return ESP_OK;
}

esp_err_t storage_set_int(const char *key, int32_t value)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to open NVS");

    ret = nvs_set_i32(handle, key, value);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to write int");

    ret = nvs_commit(handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to commit int");

    nvs_close(handle);
    return ESP_OK;
}

esp_err_t storage_get_int(const char *key, int32_t *value)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to open NVS");

    ret = nvs_get_i32(handle, key, value);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Key '%s' not found", key);
    } else {
        ESP_RETURN_ON_ERROR(ret, TAG, "Failed to read int");
    }

    nvs_close(handle);
    return ESP_OK;
}