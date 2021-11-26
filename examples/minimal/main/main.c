#include <stdio.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/FreeRTOSConfig.h"
#include "esp_timer.h"
#include "esp_private/wifi.h"
#include <string.h>
#include <dhyara/dhyara.h>


void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
	ESP_ERROR_CHECK(dhyara_wifi_init(WIFI_MODE_AP));

    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
        esp_wifi_config_espnow_rate(WIFI_IF_AP, WIFI_PHY_RATE_MCS7_SGI);
        // esp_wifi_config_80211_tx_rate(WIFI_IF_AP, WIFI_PHY_RATE_MCS7_SGI);
    #endif 

    ESP_ERROR_CHECK(dhyara_wifi_start(WIFI_MODE_AP));
    ESP_ERROR_CHECK(dhyara_espnow_init());

    dhyara_start_default_network();

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    // For high speed communication uncomment the following (Additionally disable AMPDU)
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(78));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT40));
    // ESP_ERROR_CHECK(esp_wifi_internal_set_fix_rate(WIFI_IF_AP, 1, WIFI_PHY_RATE_MCS7_SGI));
    

    // terminate the main task after configuring and starting the network
    vTaskDelete(0x0); 
}
