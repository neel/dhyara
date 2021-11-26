/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/dhyara.h"
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
    err = esp_wifi_init(&cfg);
    if(err != ESP_OK) return err;
    return esp_wifi_set_mode(mode);
}

esp_err_t dhyara_wifi_start(wifi_mode_t mode){
    if(mode == WIFI_MODE_AP){
        dhyara_ap_init();
    }else if(mode == WIFI_MODE_STA){
        dhyara_sta_init();
    }else if(mode == WIFI_MODE_APSTA){
        dhyara_apsta_init();
    }
    return esp_wifi_start();
}

esp_err_t dhyara_init(wifi_mode_t mode){
    esp_err_t err;
    err = dhyara_wifi_init(mode);
    if(err != ESP_OK) return err;
    err = dhyara_wifi_start(mode);
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
