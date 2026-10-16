idf_component_register(
    SRCS "WIFI_BLUETOOTH_STRENGTH_GRAPHER.c"
    INCLUDE_DIRS "."
    REQUIRES esp_wifi
    REQUIRES nvs_flash
    REQUIRES esp_netif
)
