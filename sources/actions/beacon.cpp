/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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


void dhyara::actions::beacon::operator()(const dhyara::address& addr, const dhyara::packets::beacon& beacon){
    if(!banned(addr)){
        if(!_link.neighbours().exists(addr.to_string())){
            _link.neighbours().add(addr.to_string(), dhyara::espnow_peer_channel);
        }
        if(!_link.universe().exists(addr.to_string())){
            _link.universe().add(addr.to_string());
        }
        _link.neighbours().neighbour(addr).update(beacon.name());
        _link.universe().peer(addr).name(beacon.name());
        _link.send_local(addr, dhyara::packets::type::acknowledgement, dhyara::packets::acknowledgement(beacon.time()));
    }
}

void dhyara::actions::beacon::broadcast(){
    std::string ssid;
    esp_wifi_get_mode(&_mode);
    if(_mode == WIFI_MODE_AP || _mode == WIFI_MODE_APSTA){
        wifi_config_t config;
        std::memset(&config, 0, sizeof(wifi_config_t));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_config(WIFI_IF_AP, &config));
        std::copy_n(config.ap.ssid, config.ap.ssid_len, std::back_inserter(ssid));
    }
    _link.send_local(dhyara::address::all(), dhyara::packets::type::beacon, dhyara::packets::beacon(ssid));
}

void dhyara::actions::beacon::ban(const dhyara::address& addr){
    if(!_banned.count(addr)){
        _banned.insert(addr);
    }
}
