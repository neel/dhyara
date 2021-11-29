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
    
	ESP_ERROR_CHECK(dhyara_init(WIFI_MODE_AP));

    dhyara_start_default_network();

    // terminate the main task after configuring and starting the network
    vTaskDelete(0x0); 
}
