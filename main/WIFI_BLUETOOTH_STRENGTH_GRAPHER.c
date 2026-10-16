#include <stdio.h>
#include <stdbool.h>

#include "esp_wifi.h"

void gather_signals(void)
{
    // Configs for WiFi behavior.
    bool block = false;
    wifi_mode_t mode = WIFI_MODE_STA;
    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_scan_config_t scan_config = {
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE
    };

    // inits driver of wifi to defualt module: case esp32s3.
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));

    // inits the wifi capibilities: case station mode.
    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
    ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));

    // Start WiFi to Configuration, it creates a control block and starts station.
    ESP_ERROR_CHECK(esp_wifi_start());

    // Start Scan, block program until scan is finished.
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, block));

}
    /*
     * esp_err_t esp_wifi_set_band(wifi_band_t band)
     *
     * esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records) 
     * Retrieve the list of APs found during the last scan. 
     * The returned AP list is sorted in descending order based on RSSI.  
     * *
     * struct wifi_scan_threshold_t 
     *         The RSSI value of the 5G AP is within the rssi_5g_adjustment range compared to the 2G AP, the 5G AP will be given priority for connection.
     *
     * struct wifi_sta_info_t
     *         uint8_t mac [6] == MAC address
     *         int8_t rssi == Current average rssi of sta connected
     *
     * enum wifi_sort_method_t
     *         enumerator WIFI_CONNECT_AP_BY_SIGNAL
     *         Sort match AP in scan list by RSSI
     *
     * enum wifi_band_mode_t
     *     enumerator WIFI_BAND_MODE_AUTO
     *
     */

// Entry point of code a ESP-IDF program -->
void app_main(void)
{
    gather_signals();

}
