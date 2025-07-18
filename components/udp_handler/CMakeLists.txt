idf_component_register(SRCS "udp_handler.c"
                    PRIV_REQUIRES esp_event
                    PRIV_REQUIRES esp_netif
                    PRIV_REQUIRES esp_wifi
                    PRIV_REQUIRES nvs_flash
                    PRIV_REQUIRES esp_driver_i2s
                    PRIV_REQUIRES esp_microphone
                    INCLUDE_DIRS "include")