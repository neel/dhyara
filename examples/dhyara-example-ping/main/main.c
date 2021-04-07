#include <stdio.h>
#include "mainx.h"
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
#include <dhyara/wifi.h>


void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
	wifi_config_t ap_config = {
		.ap = {
			.ssid_len = 0,
			.channel = 1,
			.authmode = WIFI_AUTH_OPEN,
			.max_connection = 4
		}
	};
    
	ESP_ERROR_CHECK(dhyara_wifi_init(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // { Configure WiFi
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(78));
    // }
    
    // { Configure AP Interface
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT40));
    ESP_ERROR_CHECK(esp_wifi_internal_set_fix_rate(WIFI_IF_AP, 1, WIFI_PHY_RATE_MCS7_SGI));
    // }
    
    mainx();
}
