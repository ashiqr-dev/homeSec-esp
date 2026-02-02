// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"

esp_err_t storage_init(void);
esp_err_t storage_set_int(const char *key, int32_t value);
esp_err_t storage_get_int(const char *key, int32_t *value);