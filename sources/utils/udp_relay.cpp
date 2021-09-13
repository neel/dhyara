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

#include "dhyara/utils/udp_relay.h"
#include "esp_log.h"

static constexpr const std::size_t max_length = dhyara::udp_relay_rcv_buffer_size;
static std::uint8_t _buffer[max_length];

dhyara::utils::udp_relay::udp_relay(dhyara::network& network, std::uint16_t port): _network(network), _sink(dhyara::address::null()), _port(port) {
    init();
}

dhyara::utils::udp_relay::udp_relay(dhyara::network& network, const dhyara::address& target, std::uint16_t port): _network(network), _sink(target), _port(port) {
    init();
}

int dhyara::utils::udp_relay::init(){
    _local.sin_addr.s_addr = htonl(INADDR_ANY);
    _local.sin_family = AF_INET;
    _local.sin_port = htons(_port);
    
    _remote.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    _remote.sin_family = AF_INET;
    _remote.sin_port = htons(_port);
    
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    
    return bind(_socket, (struct sockaddr*)&_local, sizeof(_local));
}


void dhyara::utils::udp_relay::run(){
    static int64_t last = esp_timer_get_time();
    static std::size_t count = 0, bytes = 0;
    while(true){
        std::fill_n(_buffer, max_length, 0);
        ssize_t bytes_recvd = receive_one();
        
        count++;
        bytes += bytes_recvd;
        std::uint64_t now = esp_timer_get_time();
        std::uint64_t diff = now - last;
        if(diff >= 1000000){
            ESP_LOGI("udp-relay", "U2D: Count %zu, Bytes %zu, Duration %lfs", count, bytes, (double)diff/1000000.0);
            last = now;
            count = 0;
            bytes = 0;
        }
    }
}

ssize_t dhyara::utils::udp_relay::receive_one(){
    char addr_str[16];
    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);
    ssize_t bytes_recvd = recvfrom(_socket, _buffer, sizeof(_buffer), 0, (struct sockaddr *)&source_addr, &socklen);
    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
    if(!bytes_recvd){
        ESP_LOGE("udp-relay", "Failed to receive from %s over UDP", addr_str);
    }else{
        ESP_LOGV("udp-relay", "Received %zu bytes from %s over UDP", bytes_recvd, addr_str);
        _network.send(_sink, _buffer, bytes_recvd);
    }
    return bytes_recvd;
}

void dhyara::utils::udp_relay::send(const dhyara::packets::data& data){
    static int64_t last = esp_timer_get_time();
    static std::size_t count = 0, bytes = 0;
    
    data.copy_to(_buffer);
    ssize_t bytes_sent = sendto(_socket, _buffer, data.length(), 0, (struct sockaddr *)&_remote, sizeof(_remote));
    if(bytes_sent < data.length()){
        ESP_LOGE("udp-relay", "Failed to broadcast the received data over UDP");
    }else{
        ESP_LOGV("udp-relay", "Broadcasted %zu bytes over UDP", bytes_sent);
    }
    
    count++;
    bytes += bytes_sent;
    std::uint64_t now = esp_timer_get_time();
    std::uint64_t diff = now - last;
    if(diff >= 1000000){
        ESP_LOGI("udp-relay", "D2U: Count %zu, Bytes %zu, Duration %lfs", count, bytes, (double)diff/1000000.0);
        last = now;
        count = 0;
        bytes = 0;
    }
}

