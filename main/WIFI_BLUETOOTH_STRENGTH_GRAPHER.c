#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "freertos/task.h"

#define SCAN_INTERVAL 5000

static const char *TAG = "Wifi Scanner";

esp_err_t init_wifi(void)
{
    esp_err_t err;

    // Configs for WiFi behavior.
    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();

    err = esp_netif_init();
    if (err != ESP_OK)
    {
        return err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK)
    {
        return err;
    }

    if (esp_netif_create_default_wifi_sta() == NULL)
    {
        ESP_LOGE(TAG, "esp_netif_create_default_wifi_sta failed to create pointer to esp-netif instance");
        return ESP_FAIL;
    }

    // inits driver of wifi to defualt module: case esp32s3.
    err = esp_wifi_init(&init_config);
    if (err != ESP_OK)
    {
        return err;
    }

    // inits the wifi capibilities: case station mode.
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK)
    {
        return err;
    }

    // Start WiFi to Configuration, it creates a control block and starts station.
    err = esp_wifi_start();
    if (err != ESP_OK)
    {
        return err;
    }


    // Log information that the initulization finished
    ESP_LOGI(TAG, "Initialisation Of WiFi: Completed Without Errors");
    return ESP_OK;
}


//LLM HELPED
static void clear_driver_scan_list(void)
{
    esp_err_t err = esp_wifi_clear_ap_list();

    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Failed to clear scan list: %s", esp_err_to_name(err));
    }
}

static esp_err_t gather_signals(void)
{
    esp_err_t err;

    // Configs for Scan behavior
    bool block = true;
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_ACTIVE
    };

    // Start Scan, block program until scan is finished.
    err = esp_wifi_scan_start(&scan_config, block);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Scan failed: %s", esp_err_to_name(err));
        return err;
    }

    // Get the number of access points found.
    uint16_t scanned_APs = 0;

    err = esp_wifi_scan_get_ap_num(&scanned_APs);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get AP count: %s", esp_err_to_name(err));
        clear_driver_scan_list();
        return err;
    }

   // LLM HELP Handle an empty scan before allocating memory.
    if (scanned_APs == 0)
    {
        ESP_LOGI(TAG, "No access points found");
        clear_driver_scan_list();
        return ESP_OK;
    }
    

    // LLM HELP
    wifi_ap_record_t *ap_infos = malloc(sizeof(*ap_infos) * scanned_APs);


    // Part LLM HELP
     if (ap_infos == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for AP records");
        free(ap_infos);
        clear_driver_scan_list();
        return ESP_ERR_NO_MEM;
    }

    uint16_t num_AP_results = scanned_APs;

    err = esp_wifi_scan_get_ap_records(&num_AP_results, ap_infos);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to retrieve AP records: %s", esp_err_to_name(err));
        free(ap_infos);
        clear_driver_scan_list();
        return err;
    }

    ESP_LOGI(TAG, "Found %u Access Points", (unsigned int)num_AP_results);
        
    // LLM HELPED
    printf("\n\n\n\nNEW SCAN!!!!!");
    for (unsigned int i = 0; i < num_AP_results; i++)
    {
        wifi_ap_record_t *ap = &ap_infos[i];
        // Prints the SSID, MAC ADDRESS, AND STRENGTH OF WIFI AP, HIGHER RSSI VALUES == STRONGER SIGNAL
        printf("SSID: %.*s | BSSID: "
                "%02X:%02X:%02X:%02X:%02X:%02X | RSSI: %d dBm\n", 
                (int)sizeof(ap->ssid), 
                (const char *)ap->ssid,
                (unsigned int)ap->bssid[0],
                (unsigned int)ap->bssid[1],
                (unsigned int)ap->bssid[2],
                (unsigned int)ap->bssid[3],
                (unsigned int)ap->bssid[4],
                (unsigned int)ap->bssid[5],
                (int)ap->rssi);
    }

    free(ap_infos);
    return ESP_OK;
}

// Entry point of code a ESP-IDF program -->
void app_main(void)
{

    // Initialize NVS straight from https://github.com/espressif/esp-idf/blob/v6.1/examples/wifi/getting_started/station/main/station_example_main.c
    // ###############################################################################
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // start wifi 
    ESP_ERROR_CHECK(init_wifi());
    // ###############################################################################

    // gather signals every x seconds
    while (true)
    {
        ret = gather_signals();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Gather Signals Failed: %s", esp_err_to_name(ret));
        }
        vTaskDelay(pdMS_TO_TICKS(SCAN_INTERVAL));
    }
}
