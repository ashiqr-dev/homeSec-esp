// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#include <string.h>

#include "storage.h"
#include "wifi_sta.h"

void app_main(void)
{
    ESP_ERROR_CHECK(storage_init());
    ESP_ERROR_CHECK(wifi_init_sta());
}