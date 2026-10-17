#include <stdio.h>
#include <stdbool.h>

#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "freertos/task.h"

#define INCLUDE_vTaskDelay 1


static const char *TAG = "wifi scanner";

void init_wifi(void)
{
    // Configs for WiFi behavior.
    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // inits driver of wifi to defualt module: case esp32s3.
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));

    // inits the wifi capibilities: case station mode.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Start WiFi to Configuration, it creates a control block and starts station.
    ESP_ERROR_CHECK(esp_wifi_start());


    // Log information that the initulization finished
    ESP_LOGI(TAG, "init finished");
}

void gather_signals(void)
{
    // Configs for Scan behavior
    bool block = true;
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_ACTIVE
    };


    // Start Scan, block program until scan is finished.
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, block));

    uint16_t max_num_APs = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&max_num_APs));
    if (max_num_APs == 0)
    {
        ESP_LOGI(TAG, "No access points found");
        ESP_ERROR_CHECK(esp_wifi_clear_ap_list());
    }
    else
    {
        wifi_ap_record_t *ap_infos = malloc((size_t)max_num_APs * sizeof(*ap_infos));

        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&max_num_APs, ap_infos));

        printf("\n\n\n\nNEW SCAN!!!!!");
        for (int i = 0; i < max_num_APs; i++)
        {
            // prints the SSID, MAC ADDRESS, AND STRENGTH OF WIFI AP, HIGHER RSSI VALUES == STRONGER SIGNAL
            printf("SSID: %s BSSID: %02X:%02X:%02X:%02X:%02X:%02X: RSSI: %d\n", 
                    (char *)ap_infos[i].ssid, 
                    ap_infos[i].bssid[0], 
                    ap_infos[i].bssid[1], 
                    ap_infos[i].bssid[2], 
                    ap_infos[i].bssid[3], 
                    ap_infos[i].bssid[4], 
                    ap_infos[i].bssid[5], 
                    (int)ap_infos[i].rssi);
        }

        ESP_ERROR_CHECK(esp_wifi_clear_ap_list());
        free(ap_infos);
    }
}



// Entry point of code a ESP-IDF program -->
void app_main(void)
{

    //Initialize NVS straight from https://github.com/espressif/esp-idf/blob/v6.1/examples/wifi/getting_started/station/main/station_example_main.c
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // start wifi 
    init_wifi();

    // gather signals every x seconds
    while (true)
    {
        gather_signals();
        vTaskDelay(pdMS_TO_TICKS(500));
    }


    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
}
