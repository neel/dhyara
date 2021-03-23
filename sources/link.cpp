/*
 * Copyright (c) 2021, Neel Basu <neel.basu.z@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY Neel Basu <neel.basu.z@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Neel Basu <neel.basu.z@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "dhyara/link.h"
#include "esp_wifi.h"
#include <functional>
#include <cassert>
#include "esp_log.h"

struct ieee_802_11_management_frame{
  int16_t fctl;
  int16_t duration;
  uint8_t destination[6];
  uint8_t source[6];
  uint8_t bssid[6];
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed));

dhyara::link::link(): 
    _fifo_rcv(std::bind(&dhyara::link::q_receive, this, std::placeholders::_1, std::placeholders::_2)), 
    _mac(dhyara::peer_address::null())
{}


void dhyara::link::init(){
    std::uint8_t base_mac[6];
    esp_wifi_get_mac(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), base_mac);
    _mac.set(base_mac);
    _neighbours.add(dhyara::peer::address::all(), dhyara::espnow_broadcast_channel);
}


bool dhyara::link::_transmit(const std::uint8_t* dest, const std::uint8_t* data, std::size_t len){
    static std::uint8_t broadcast_addr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    esp_err_t error = esp_now_send(dest ? dest : broadcast_addr, data, len);
    if(error != ESP_OK){
        ESP_LOGE("dhyara", "send failed %s", esp_err_to_name(error));
        return false;
    }
    return true;
}

bool dhyara::link::transmit(const dhyara::peer_address& addr, const dhyara::frame& frame){
    if(_neighbours.exists(addr)){
        bool success = _queue_snd.en(dhyara::message(addr, frame));
        if(success){
            auto it = _counters.find(frame.type());
            if(it != _counters.end()){
                it->second.first++;
            }
#if DHYARA_ENABLED_SEND_QUEUEING
            if(_notifications.empty()){
                // Wake up the sleeping send task
                _notifications.en(0);
            }
#endif
        }
        return success;
    }
    ESP_LOGE("dhyara", "peer %s unreachable", addr.to_string().c_str());
    return false;
}

void dhyara::link::_esp_sent_cb(const uint8_t*, esp_now_send_status_t){
    static std::uint8_t buffer[sizeof(dhyara::frame)];
    if(_queue_snd.de(_msg_dequeued, 0)){
        dhyara::write(_msg_dequeued.frame, buffer);
        _transmit(_msg_dequeued.address.raw(), buffer, _msg_dequeued.frame.size());
    }
}

void dhyara::link::_esp_promiscous_rx_cb(void* buffer, wifi_promiscuous_pkt_type_t type){
    static dhyara::delay_type last = esp_timer_get_time();
    dhyara::delay_type now = esp_timer_get_time();
    if((now - last) <= 1000){ // don't update rssi within 1ms
        return;
    }
    wifi_promiscuous_pkt_t* p = (wifi_promiscuous_pkt_t*)buffer;
    ieee_802_11_management_frame* ether = (ieee_802_11_management_frame*)p->payload;
    if(p->rx_ctrl.sig_len >= sizeof(ieee_802_11_management_frame)){
        dhyara::peer_address source(ether->source);
        if(_neighbours.exists(source)){
            _neighbours.get_peer(source).rssi(p->rx_ctrl.rssi);
            last = now;
        }
    }
}


void dhyara::link::_esp_rcvd_cb(const uint8_t* source, const uint8_t* data, int len){
    dhyara::read(_frame_rcv, data, len);
    dhyara::peer_address addr(source);
    _fifo_rcv.enqueue(addr, _frame_rcv);
    _frame_rcv.clear();
}

void dhyara::link::install(dhyara::packets::type type, dhyara::link::callback_type cb){
    _handlers.insert(std::make_pair(type, cb));
    _counters.insert(std::make_pair(type, std::make_pair(0, 0)));
}

void dhyara::link::q_receive(const dhyara::peer_address& address, const dhyara::frame& frame){
    auto cit = _counters.find(frame.type());
    if(cit != _counters.end()){
        cit->second.second++;
    }
    
    auto hit = _handlers.find(frame.type());
    if(hit != _handlers.end()){
        hit->second(address, frame);
    }else{
        ESP_LOGE("dhyara", "No action installed for frame type %d", (int)frame.type());
    }
}

void dhyara::link::start_rcv(std::size_t ticks){
    _fifo_rcv.loop(ticks);
}

void dhyara::link::start_snd(std::size_t ticks){
    // 1. start_snd sleeps initially (State: _notifications queue is empty)
    // 2. woken up by transmit() function (transmit enqueues a notification to wake it up)
    // 3. once woken up dequeues (empties) _notifications and synthetically calls _esp_sent_cb (which is also the send callback) and sleeps
    // 4. _esp_sent_cb *tries* to dequeue a single element (if exists) from _queue_snd and transmits using the transmit() function 
    // 5. The transmission results into subsequent call to _esp_sent_cb (as it is the send callback) -> loop to 4
    // 6. Once _queue_snd is empty the (4 -> 5) loop terminates (State: _notifications queue is empty, start_snd is sleeping since 3) -> loop to 1
#if DHYARA_ENABLED_SEND_QUEUEING
    char dummy;
    while(_notifications.de(dummy, ticks)){
        _esp_sent_cb(0x0, ESP_NOW_SEND_SUCCESS);
    }
#endif 
}

void dhyara::link::reset(dhyara::packets::type type){
    auto it = _counters.find(type);
    if(it != _counters.end()){
        it->second = std::pair<std::size_t, std::size_t>(0, 0);
    }
}

std::size_t dhyara::link::tx(dhyara::packets::type type) const{
    auto it = _counters.find(type);
    if(it != _counters.end()){
        return it->second.first;
    }
    return 0;
}

std::size_t dhyara::link::rx(dhyara::packets::type type) const{
    auto it = _counters.find(type);
    if(it != _counters.end()){
        return it->second.second;
    }
    return 0;
}

std::int8_t dhyara::link::max_rssi() const{
    std::int8_t max = -127;
    for(auto it = _neighbours.begin(); it != _neighbours.end(); ++it){
        max = std::max(max, it->second.rssi());
    }
    return max;
}

dhyara::delay_type dhyara::link::lost() const{
    return esp_timer_get_time() - _routes.lost_since();
}
