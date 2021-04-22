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

#include "dhyara/neighbour.h"

dhyara::neighbour::neighbour(const std::string& addr, std::uint8_t ch, bool enc): dhyara::neighbour::neighbour(dhyara::address(addr)){}

dhyara::neighbour::neighbour(const dhyara::address& addr, std::uint8_t ch, bool enc): peer(addr), _rssi(0){
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
