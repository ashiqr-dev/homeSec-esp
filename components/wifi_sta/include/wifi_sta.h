// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"

/**
 * @brief Initialize WiFi in Station (STA) mode and connect to configured network
 * Reads SSID and password from menuconfig settings
 * @return ESP_OK on successful connection, error code otherwise
 */
esp_err_t wifi_init_sta(void);