#include <stdint.h>
#include <stdio.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/FreeRTOSConfig.h"
#include "esp_timer.h"
#include "esp_private/wifi.h"
#include <string.h>
#include <dhyara/dhyara.h>

uint8_t node_a[] = {0x4c, 0x11, 0xae, 0x9c, 0x1a, 0xc9};
uint8_t node_b[] = {0x4c, 0x11, 0xae, 0x71, 0x0f, 0x4d};
uint8_t node_c[] = {0x4c, 0x11, 0xae, 0x9c, 0xaa, 0x29};
uint8_t node_d[] = {0xac, 0x67, 0xb2, 0x25, 0x8e, 0xa5};
uint8_t node_e[] = {0xac, 0x67, 0xb2, 0x0b, 0xb8, 0x65};
uint8_t node_f[] = {0x8c, 0xaa, 0xb5, 0x94, 0x5e, 0x41};

uint8_t* nodes[] = {node_a, node_b, node_c, node_d, node_e, node_f};

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

    /*for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 6; ++j){
            int d = j - i;
            if(d < -1 || d > 1){
                ESP_LOGI("ad-hoc", "node %d ("MACSTR") isolating node %d ("MACSTR")", i, MAC2STR(nodes[i]), j, MAC2STR(nodes[j]));
                dhyara_isolate(nodes[i], nodes[j]);
            }
        }
    }*/

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    // For high speed communication uncomment the following (Additionally disable AMPDU)
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(78));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT40));
    ESP_ERROR_CHECK(esp_wifi_internal_set_fix_rate(WIFI_IF_AP, 1, WIFI_PHY_RATE_MCS7_SGI));

    // terminate the main task after configuring and starting the network
    vTaskDelete(0x0); 
}
