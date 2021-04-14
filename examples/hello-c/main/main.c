#include <stdio.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
#include <dhyara/dhyara.h>

void data_received(const unsigned char* source, const void* data, unsigned long len){
    ESP_LOGI("hello-c", "data received \"%s\" (length %lu)", (const char*)data, len);
}

void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
	ESP_ERROR_CHECK(dhyara_init(WIFI_MODE_AP));
    dhyara_start_default_network();
    
    dhyara_receivef(&data_received);

    uint8_t self[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    dhyara_local(self);
    
    ESP_LOGI("hello-c", "Local MAC address %x:%x:%x:%x:%x:%x", self[0], self[1], self[2], self[3], self[4], self[5]);
    
    uint8_t source[] = {0x4c, 0x11, 0xae, 0x71, 0x0f, 0x4d};
    uint8_t sink[]   = {0x4c, 0x11, 0xae, 0x9c, 0xa6, 0x85};
    
    const uint8_t* other = 0x0;
    
    if(memcmp(self, source, 6) == 0) other = sink;
    if(memcmp(self, sink,   6) == 0) other = source;
    
    while(1){
        if(other){
            dhyara_ping(other, .count = 1, .batch = 10);
            dhyara_traceroute(other);
            dhyara_send(other, "Hello World");
            dhyara_send(other, "Hello World", 5);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
