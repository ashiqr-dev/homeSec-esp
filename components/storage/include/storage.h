// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"

/**
 * @brief Initialize NVS (Non-Volatile Storage) for persistent data storage
 * Creates or opens the application namespace
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t storage_init(void);

/**
 * @brief Write an integer value to NVS storage
 * @param key The key to store the value under
 * @param value The integer value to store
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t storage_set_int(const char *key, int32_t value);

/**
 * @brief Read an integer value from NVS storage
 * @param key The key to read from
 * @param value Pointer to store the retrieved value
 * @return ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if key doesn't exist, error code otherwise
 */
esp_err_t storage_get_int(const char *key, int32_t *value);