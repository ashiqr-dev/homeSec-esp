#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "wifi_sta.h"

#define WIFI_MAXIMUM_RETRY 5

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0

#define WIFI_FAIL_BIT BIT1

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASS CONFIG_WIFI_PASS

static const char *TAG = "wifi_sta";
static int s_retry_num = 0;

static esp_err_t wifi_init_event_group(void);
static esp_err_t wifi_driver_init(void);
static esp_err_t wifi_register_event_handlers(void);
static esp_err_t wifi_config_sta(void);
static esp_err_t wifi_sta_start(void);
static void wifi_wait_for_connection(void);

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retrying connection to AP...");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGW(TAG, "Connection to the AP failed");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// clang-format off
esp_err_t wifi_init_sta(void)
{
    ESP_RETURN_ON_ERROR(wifi_init_event_group(),        TAG, "Event group init failed");
    ESP_RETURN_ON_ERROR(wifi_driver_init(),             TAG, "Event group init failed");
    ESP_RETURN_ON_ERROR(wifi_register_event_handlers(), TAG, "Event handlers registration failed");
    ESP_RETURN_ON_ERROR(wifi_config_sta(),              TAG, "Wi-Fi config failed");
    ESP_RETURN_ON_ERROR(wifi_sta_start(),               TAG, "Wi-Fi start failed");
    
    wifi_wait_for_connection();

    ESP_LOGI(TAG, "wifi_init_sta() finished");

    return ESP_OK;
}
// clang-format on

/************************************************
** Wi-Fi helpers
*************************************************/

static esp_err_t wifi_init_event_group(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "Failed to init netif");

    ESP_RETURN_ON_ERROR(esp_event_loop_create_default(),
                        TAG,
                        "Event loop default creation failed");
    esp_netif_create_default_wifi_sta();

    return ESP_OK;
}

static esp_err_t wifi_driver_init(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    return esp_wifi_init(&cfg);
}

static esp_err_t wifi_register_event_handlers(void)
{
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_RETURN_ON_ERROR(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &wifi_event_handler,
                                                            NULL,
                                                            &instance_any_id),
                        TAG,
                        "Wi-Fi event handler registration failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &wifi_event_handler,
                                                            NULL,
                                                            &instance_got_ip),
                        TAG,
                        "IP event handler registration failed");
    return ESP_OK;
}

static esp_err_t wifi_config_sta(void)
{
    wifi_config_t wifi_config = {
        .sta = {.ssid = WIFI_SSID,
                .password = WIFI_PASS,
                .threshold.authmode = WIFI_AUTH_WPA2_PSK}};

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA),
                        TAG,
                        "Set STA mode failed");

    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config),
                        TAG,
                        "Wi-Fi config failed");
    return ESP_OK;
}

static esp_err_t wifi_sta_start(void)
{
    return esp_wifi_start();
}

static void wifi_wait_for_connection(void)
{
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to AP SSID: %s", WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Failed to connect to SSID: %s", WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}