// Copyright (c) 2026 Ashiq Renju
// SPDX-License-Identifier: MIT
#include "telegram_bot.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "sdkconfig.h"
#include "storage.h"
#include "string.h"

extern const uint8_t ca_pem_start[] asm("_binary_ca_pem_start");

static const char *TAG = "telegram_bot";
#define BOT_TOKEN CONFIG_BOT_TOKEN

static char response_buffer[2048];
static int response_len = 0;

typedef struct {
    int64_t last_update_id;
} telegram_bot_t;

static telegram_bot_t telegram_bot = {.last_update_id = 0};

static void process_update(cJSON *update);
static void process_updates(const char *json_str);

static telegram_message_callback_t message_callback = NULL;

esp_err_t telegram_bot_init(telegram_message_callback_t callback)
{
    if (callback == NULL) {
        ESP_LOGE(TAG, "Callback cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    message_callback = callback;
    ESP_LOGI(TAG, "Telegram bot initialized");
    return ESP_OK;
}

static esp_err_t http_event_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ON_CONNECTED:
        response_len = 0;
        memset(response_buffer, 0, sizeof(response_buffer));
        ESP_LOGI(TAG, "HTTP Connected");
        break;

    case HTTP_EVENT_ON_DATA:
        if (response_len + evt->data_len < sizeof(response_buffer)) {
            memcpy(response_buffer + response_len, evt->data, evt->data_len);
            response_len += evt->data_len;
            response_buffer[response_len] = '\0';
        } else {
            ESP_LOGW(TAG, "Response buffer too small!");
        }
        break;

    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP Finished");
        process_updates(response_buffer);
        break;

    default:
        break;
    }
    return ESP_OK;
}

esp_err_t telegram_bot_get_updates(void)
{
    char url[256];
    snprintf(url,
             sizeof(url),
             "https://api.telegram.org/bot%s/getUpdates?timeout=%d&offset=%lld&limit=%d",
             BOT_TOKEN,
             30,
             telegram_bot.last_update_id > 0 ? (long long)(telegram_bot.last_update_id + 1) : 0LL,
             1);

    ESP_LOGI(TAG, "Requesting: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .cert_pem = (const char *)ca_pem_start,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .timeout_ms = 35000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS GET Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTPS GET request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    esp_http_client_cleanup(client);
    return ESP_OK;
}

esp_err_t telegram_bot_send_message(int64_t chat_id, const char *text)
{
    char url[256];
    snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/sendMessage", BOT_TOKEN);

    // Build JSON payload
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "chat_id", (double)chat_id);
    cJSON_AddStringToObject(json, "text", text);
    char *json_str = cJSON_PrintUnformatted(json);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)ca_pem_start,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_str, strlen(json_str));

    esp_err_t err = esp_http_client_perform(client);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send message: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    cJSON_free(json_str);
    cJSON_Delete(json);

    return err;
}

esp_err_t telegram_bot_check_status(void)
{
    char url[256];
    snprintf(url,
             sizeof(url),
             "https://api.telegram.org/bot%s/getMe",
             BOT_TOKEN);

    ESP_LOGI(TAG, "Requesting: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .cert_pem = (const char *)ca_pem_start,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int16_t get_status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTPS GET Status = %d, content_length = %d", get_status, esp_http_client_get_content_length(client));
        if (get_status == 404) {
            ESP_LOGE(TAG, "Please enter a valid Telegram bot token!");
            return ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG, "HTTPS GET request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    esp_http_client_cleanup(client);
    return ESP_OK;
}

/************************************************
** GET request helpers
*************************************************/

// Helper function to process a single update
static void process_update(cJSON *update)
{
    cJSON *update_id_json = cJSON_GetObjectItem(update, "update_id");
    if (update_id_json == NULL) {
        return;
    }

    // Update offset
    int64_t update_id = (int64_t)cJSON_GetNumberValue(update_id_json);
    if (update_id > telegram_bot.last_update_id) {
        telegram_bot.last_update_id = update_id;
        ESP_LOGI(TAG, "Updated offset to: %lld", (long long)(telegram_bot.last_update_id + 1));
    }

    // Process message
    cJSON *message = cJSON_GetObjectItem(update, "message");
    if (message == NULL) {
        return;
    }

    cJSON *text = cJSON_GetObjectItem(message, "text");
    cJSON *from = cJSON_GetObjectItem(message, "from");
    cJSON *chat = cJSON_GetObjectItem(message, "chat");

    if (text != NULL && from != NULL) {
        cJSON *first_name = cJSON_GetObjectItem(from, "first_name");
        cJSON *chat_id_json = cJSON_GetObjectItem(chat, "id");
        int64_t chat_id = chat_id_json ? (int64_t)cJSON_GetNumberValue(chat_id_json) : 0;
        ESP_LOGI(TAG, "Message from %s: \n%s", first_name ? first_name->valuestring : "Unknown", text->valuestring);

        if (message_callback != NULL) {
            message_callback(chat_id, text->valuestring, first_name ? first_name->valuestring : "Unknown");
        }
    }
}

// Helper function to process all updates
static void process_updates(const char *json_str)
{
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return;
    }

    cJSON *result = cJSON_GetObjectItem(json, "result");
    if (result == NULL || !cJSON_IsArray(result)) {
        ESP_LOGI(TAG, "No result array in response");
        cJSON_Delete(json);
        return;
    }

    int count = cJSON_GetArraySize(result);
    ESP_LOGI(TAG, "Got %d updates", count);

    for (int i = 0; i < count; i++) {
        cJSON *update = cJSON_GetArrayItem(result, i);
        process_update(update);
    }

    cJSON_Delete(json);
}