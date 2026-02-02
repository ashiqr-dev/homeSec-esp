idf_component_register(SRCS "telegram_bot.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_http_client esp-tls cjson storage
                    EMBED_TXTFILES "certs/ca.pem")