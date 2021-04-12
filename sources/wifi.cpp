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
#include "dhyara/network.h"
#include "dhyara/packets/data.h"
#include "dhyara/peer.h"
#include "dhyara/tools/ping.h"
#include "dhyara/tools/traceroute.h"
#include "esp_wifi.h"
#include <cstring>
#include <cstdarg>

#include <iostream>

static dhyara::link g_dhyara_link;
static dhyara::network* g_dhyara_network_ptr = 0x0;

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

esp_err_t dhyara_espnow_init(){
    esp_err_t err;
    err = esp_now_init();
    if(err != ESP_OK) return err;
    err = esp_now_register_send_cb(_dhyara_sent);
    if(err != ESP_OK) return err;
    err = esp_now_register_recv_cb(_dhyara_rcvd);
    if(err != ESP_OK) return err;
    err = esp_wifi_set_promiscuous_filter(&g_dhyara_promiscous_filter);
    if(err != ESP_OK) return err;
    err = esp_wifi_set_promiscuous_rx_cb(&_dhyara_promiscuous);
    if(err != ESP_OK) return err;
    err = esp_wifi_set_promiscuous(true);
    return err;
    // ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_csi, 0x0));
    // ESP_ERROR_CHECK(esp_wifi_set_csi(true));
}

dhyara::link& dhyara_link(){
    return g_dhyara_link;
}

void dhyara_create_default_network(){
    dhyara_set_default_network(new dhyara::network(g_dhyara_link));
}

void dhyara_set_default_network(dhyara::network* network){
    if(g_dhyara_network_ptr){
        delete g_dhyara_network_ptr;
        g_dhyara_network_ptr = 0x0;
    }
    g_dhyara_network_ptr = network;
}

dhyara::network* dhyara_get_default_network(){
    return g_dhyara_network_ptr;
}

bool dhyara_has_default_network(){
    return !!g_dhyara_network_ptr;
}

void dhyara_start_default_network(){
    if(!g_dhyara_network_ptr){
        dhyara_create_default_network();
    }
    g_dhyara_network_ptr->start();
}

bool dhyara_local(unsigned char* address){
    if(dhyara_has_default_network()){
        dhyara_get_default_network()->link().address().copy(address);
        return true;
    }
    address = 0x0;
    return false;
}

bool dhyara_receive(dhyara_data_callback_t callback){
    if(dhyara_has_default_network()){
        dhyara_get_default_network()->on_data([callback](const dhyara::peer::address& source, const dhyara::packets::data& data){
            callback(data.source().raw(), data.raw(), data.length());
        });
        return true;
    }
    return false;
}

bool dhyara_send_internal(const unsigned char* target, const void* data, unsigned len){
    if(dhyara_has_default_network()){
        std::cout << "len1 " << len << std::endl;
        
        if(!len){
            len = std::strlen(reinterpret_cast<const char*>(data));
        }
        
        std::cout << "len2 " << len << std::endl;
        
        const unsigned char* buffer = reinterpret_cast<const unsigned char*>(data);
        dhyara::peer_address address(target);
        return dhyara_get_default_network()->send(address, buffer, len);
    }
    return false;
}

bool dhyara_send_ping(const unsigned char* target, uint8_t count, int8_t batch, uint8_t sleep){
    if(dhyara_has_default_network()){
        dhyara::peer_address other(target);
        dhyara::tools::ping ping(*dhyara_get_default_network());
        ping.count(count).batch(batch).sleep(sleep);
        ping(other);
        vTaskDelay(pdMS_TO_TICKS(2000));
        return true;
    }
    return false;
}

bool dhyara_traceroute(const unsigned char* target){
    if(dhyara_has_default_network()){
        dhyara::peer_address other(target);
        dhyara::tools::traceroute traceroute(*dhyara_get_default_network(), other);
        traceroute();
        vTaskDelay(pdMS_TO_TICKS(2000));
        return true;
    }
    return false;
}
