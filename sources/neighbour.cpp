/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/neighbour.h"

dhyara::neighbour::neighbour(const std::string& addr, std::uint8_t ch, bool enc): dhyara::neighbour::neighbour(dhyara::address(addr), ch, enc){}

void dhyara::neighbour::update(const std::string& n) {
    dhyara::peer::name(n);
    _last_beacon = esp_timer_get_time();
    ++_beacons;
}

void dhyara::neighbour::update_acknowledged() {
    _last_ack = esp_timer_get_time();
    ++_acknowledgements;
}

dhyara::neighbour::neighbour(const dhyara::address& addr, std::uint8_t ch, bool enc): peer(addr), _rssi(0), _born(esp_timer_get_time()), _last_beacon(_born), _last_ack(_born), _beacons(0), _acknowledgements(0){
    std::fill_n(reinterpret_cast<std::uint8_t*>(&_peer), sizeof(esp_now_peer_info_t), 0);
    const std::uint8_t* data = addr.raw();
    std::copy(data, data+6, _peer.peer_addr);
    _peer.ifidx = static_cast<wifi_interface_t>(ESP_IF_WIFI_AP);
    channel(ch);
    encrypt(enc);
}


std::ostream& dhyara::operator<<(std::ostream& os, const dhyara::neighbour& peer){
    os << "Peer< " << peer.addr() << ", Ch #" << peer.channel() << " >";
    return os;
}
