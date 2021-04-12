/*
 * Copyright (c) 2021, <copyright holder> <email>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "dhyara/wifi.h"
#include "esp_log.h"
#include "esp_wifi_types.h"
#include <assert.h>

wifi_promiscuous_filter_t g_dhyara_promiscous_filter = {
    .filter_mask = WIFI_PKT_CTRL
};

static void dhyara_ap_init(){
    esp_netif_t* netif_ap = esp_netif_create_default_wifi_ap();
    assert(netif_ap);
}

static void dhyara_sta_init(){
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
}

static void dhyara_apsta_init(){
    dhyara_ap_init();
    dhyara_sta_init();
}

static void dhyara_wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI("DHYARA AP", "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI("DHYARA AP", "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

esp_err_t dhyara_wifi_init(wifi_mode_t mode){
    esp_err_t err;
    err = esp_netif_init();
    if(err != ESP_OK) return err;
    err = esp_event_loop_create_default();
    if(err != ESP_OK) return err;
	err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &dhyara_wifi_event_handler, NULL);
    if(err != ESP_OK) return err;
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     cfg.ampdu_rx_enable = 0;
//     cfg.ampdu_tx_enable = 0;
    err = (esp_wifi_init(&cfg));
    if(err != ESP_OK) return err;
    
    if(mode == WIFI_MODE_AP){
        dhyara_ap_init();
    }else if(mode == WIFI_MODE_STA){
        dhyara_sta_init();
    }else if(mode == WIFI_MODE_APSTA){
        dhyara_apsta_init();
    }
    return esp_wifi_set_mode(mode);
}

esp_err_t dhyara_init(wifi_mode_t mode){
    esp_err_t err;
    err = dhyara_wifi_init(mode);
    if(err != ESP_OK) return err;
    err = esp_wifi_start();
    if(err != ESP_OK) return err;
    err = dhyara_espnow_init();
    return err;
}


esp_err_t dhyara_station_join(wifi_config_t* sta_config){
    esp_err_t err;
    err = esp_wifi_set_config(WIFI_IF_STA, sta_config);
    if(err != ESP_OK) return err;
    return esp_wifi_connect();
}

bool dhyara_ping(const unsigned char* target, ...){
    va_list args;
    va_start(args, target);
    
    uint8_t count = 254;
    int8_t  batch = 1;
    uint8_t sleep = 15;
    
    count = va_arg(args, unsigned);
    if(!count){
        count = 254;
        va_end(args);
        return dhyara_send_ping(target, count, batch, sleep);
    }
    
    batch = va_arg(args, int);
    if(!batch){
        batch = 1;
        va_end(args);
        return dhyara_send_ping(target, count, batch, sleep);
    }
    
    sleep = va_arg(args, unsigned);
    if(!sleep){
        sleep = 15;
        va_end(args);
        return dhyara_send_ping(target, count, batch, sleep);
    }
    
    va_end(args);
    return dhyara_send_ping(target, count, batch, sleep);
}


bool dhyara_send(const unsigned char* target, const void* data, ...){
    va_list args;
    unsigned len = 0;
    va_start(args, data);
    len = va_arg(args, int);
    va_end(args);
    
    return dhyara_send_internal(target, data, len);
}
