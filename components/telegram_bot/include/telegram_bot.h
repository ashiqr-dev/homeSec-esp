// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"

typedef void (*telegram_message_callback_t)(int64_t chat_id, const char *text, const char *from_name);
void telegram_bot_init(telegram_message_callback_t callback);
esp_err_t telegram_bot_get_updates(void);
esp_err_t telegram_bot_send_message(int64_t chat_id, const char *text);