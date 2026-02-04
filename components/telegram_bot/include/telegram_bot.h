// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"

/**
 * @brief Callback function for handling received Telegram messages
 * @param chat_id The chat ID from which the message was received
 * @param text The message text content
 * @param from_name The name of the message sender
 */
typedef void (*telegram_message_callback_t)(int64_t chat_id, const char *text, const char *from_name);

/**
 * @brief Initialize the Telegram bot with a message callback
 * @param callback Function pointer to handle incoming messages
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if callback is NULL
 */
esp_err_t telegram_bot_init(telegram_message_callback_t callback);

/**
 * @brief Check whether the bot is configured and ready.
 *
 * @return ESP_OK if ready, otherwise an error code.
 */
esp_err_t telegram_bot_check_status(void);

/**
 * @brief Fetch updates from Telegram API
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t telegram_bot_get_updates(void);

/**
 * @brief Send a message via Telegram
 * @param chat_id The chat ID to send the message to
 * @param message The message text to send
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t telegram_bot_send_message(int64_t chat_id, const char *text);