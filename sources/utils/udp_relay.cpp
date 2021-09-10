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

static constexpr const std::size_t max_length = 4096;
static std::uint8_t _buffer[max_length];

dhyara::utils::udp_relay::udp_relay(dhyara::network& network, std::uint16_t port):
    _network(network),
    _sink(dhyara::address::null()),
    _port(port),
    _socket(_io, asio::ip::udp::endpoint(asio::ip::udp::v4(), _port))
    {
        receive();
    }

dhyara::utils::udp_relay::udp_relay(dhyara::network& network, const dhyara::address& target, std::uint16_t port):
    _network(network),
    _sink(target), 
    _port(port),
    _socket(_io, asio::ip::udp::endpoint(asio::ip::udp::v4(), _port))
    {
        receive();
    }

std::size_t dhyara::utils::udp_relay::run(){
    return _io.run();
}

void dhyara::utils::udp_relay::receive(){
    asio::ip::udp::endpoint endpoint;
    _socket.async_receive_from(asio::buffer(_buffer, max_length), endpoint, [this](std::error_code ec, std::size_t bytes_recvd){
        static int64_t last = esp_timer_get_time();
        static std::size_t count = 0, bytes = 0;
        
        count++;
        bytes += bytes_recvd;
        std::uint64_t now = esp_timer_get_time();
        std::uint64_t diff = now - last;
        if(diff >= 1000000){
            ESP_LOGI("udp-relay", "U2D: Count %zu, Bytes %zu, Duration %" PRId64 " us", count, bytes, diff);
            last = now;
            count = 0;
            bytes = 0;
        }
        
        if(!ec && bytes_recvd > 0){
            ESP_LOGV("udp-relay", "Received %zu bytes from UDP to Dhyara", bytes_recvd);
            _network.send(_sink, _buffer, bytes_recvd);
        }else{
            ESP_LOGE("udp-relay", "Error %d encountered while receiving: %s", ec.value(), ec.message().c_str());
        }
        receive();
    });
}

void dhyara::utils::udp_relay::send(const dhyara::packets::data& data){
    data.copy(_buffer);
    _socket.async_send_to(asio::buffer(_buffer, data.length()), asio::ip::udp::endpoint(asio::ip::address_v4::broadcast(), _port), [this](std::error_code ec, std::size_t bytes_sent){
        ESP_LOGV("udp-relay", "Sent %zu bytes from Dhyara to UDP", bytes_sent);
        // receive();
    });
}
