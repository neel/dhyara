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

#include "dhyara/packets/beacon.h"
#include "dhyara/link.h"
#include "dhyara/peer.h"
#include "dhyara/actions/beacon.h"
#include "dhyara/packets/acknowledgement.h"
#include <esp_wifi.h>
#include <cstring>

dhyara::actions::beacon::beacon(dhyara::link& link): _link(link){
    
}


void dhyara::actions::beacon::operator()(const dhyara::peer_address& addr, const dhyara::packets::beacon& beacon){
    if(!_link.neighbours().exists(addr.to_string())){
        _link.neighbours().add(addr.to_string(), dhyara::espnow_peer_channel);
    }
    _link.neighbours().get_peer(addr).name(beacon.name());
    if(!banned(addr)){
        _link.send_local(addr, dhyara::packets::type::acknowledgement, dhyara::packets::acknowledgement(beacon.time()));
    }
}

void dhyara::actions::beacon::broadcast(){
    std::string ssid;
    esp_wifi_get_mode(&_mode);
    if(_mode == WIFI_MODE_AP || _mode == WIFI_MODE_APSTA){
        wifi_config_t config;
        std::memset(&config, 0, sizeof(wifi_config_t));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_config(ESP_IF_WIFI_AP, &config));
        std::copy_n(config.ap.ssid, config.ap.ssid_len, std::back_inserter(ssid));
    }
    _link.send_local(dhyara::peer::address::all(), dhyara::packets::type::beacon, dhyara::packets::beacon(ssid));
}

void dhyara::actions::beacon::ban(const dhyara::peer_address& addr){
    if(!_banned.count(addr)){
        _banned.insert(addr);
    }
}
