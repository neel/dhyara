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
#include "dhyara/link.h"
#include "esp_wifi.h"
#include <cstring>

static dhyara::link g_dhyara_link;

static void _dhyara_sent(const uint8_t* target, esp_now_send_status_t status){
    g_dhyara_link._esp_sent_cb(target, status);
}

static void _dhyara_rcvd(const uint8_t* source, const uint8_t* data, int len){
    g_dhyara_link._esp_rcvd_cb(source, data, len);
}

static void _dhyara_promiscuous(void* buffer, wifi_promiscuous_pkt_type_t type){
    g_dhyara_link._esp_promiscous_rx_cb(buffer, type);
}

// static void _dhyara_csi(void* ctx, wifi_csi_info_t* data){
//     dhyara::peer_address source(data->mac);
//     std::cout << source << " " << data->rx_ctrl.rssi << std::endl;
// }

void dhyara_espnow_init(){
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(_dhyara_sent));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(_dhyara_rcvd));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&g_dhyara_promiscous_filter));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&_dhyara_promiscuous));
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    // ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_csi, 0x0));
    // ESP_ERROR_CHECK(esp_wifi_set_csi(true));
}

dhyara::link& dhyara_link(){
    return g_dhyara_link;
}
