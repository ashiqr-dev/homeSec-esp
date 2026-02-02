// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#include "sdkconfig.h"

#if !defined(CONFIG_WIFI_SSID) || !defined(CONFIG_WIFI_PASS)
#error "ERROR: Wi-Fi credentials not set, run `idf.py menuconfig` to set them."
#endif

#ifndef CONFIG_BOT_TOKEN
#error "ERROR: Telegram Bot Token not set, run `idf.py menuconfig` to set them."
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "storage.h"
#include "telegram_bot.h"
#include "wifi_sta.h"

static const char *TAG = "main";

void on_telegram_message(int64_t chat_id, const char *text, const char *from_name)
{
    if (strcmp(text, "/start") == 0) {
        telegram_bot_send_message(chat_id, "Hello! this is the ESP32!");
    }
}

void telegram_bot_task(void *pvParameters)
{
    for (;;) {
        esp_err_t err = telegram_bot_get_updates();
        if (err != ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(storage_init());
    ESP_ERROR_CHECK(wifi_init_sta());

    telegram_bot_init(on_telegram_message); // Register the event handler
    xTaskCreate(telegram_bot_task,
                "telegram_bot_task",
                8192,
                NULL,
                5,
                NULL);

    ESP_LOGI(TAG, "Telegram bot started");
}