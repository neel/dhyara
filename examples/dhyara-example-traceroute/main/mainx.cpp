#include "mainx.h"
#include <dhyara/network.h>
#include <iostream>
#include <freertos/FreeRTOS.h>
#include <dhyara/link.h>
#include "application.h"

dhyara::link g_link;

void _sent(const uint8_t* target, esp_now_send_status_t status){
    g_link._esp_sent_cb(target, status);
}

void _rcvd(const uint8_t* source, const uint8_t* data, int len){
    g_link._esp_rcvd_cb(source, data, len);
}

void mainx(){
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(_sent));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(_rcvd));
    
    dhyara::network network(g_link);
    network.start();
    
    esp_log_level_set("dhyara", ESP_LOG_VERBOSE);
    
    application app(network);
    app.main();
} 
