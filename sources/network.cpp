/*
 * Copyright (c) 2019, <copyright holder> <email>
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

#include "dhyara/network.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

dhyara::network::network(dhyara::link& link): 
    _link(link),
    _synchronizer(_link), 
    _beacon(_link), 
    _acknowledgement(_link, _synchronizer), 
    _advertisement(_link, _synchronizer),
    _echo_request(_link),
    _echo_reply(_link),
    _echo_lost(_link),
    _chunk(_link),
    _delivered(_link)
    {}

void dhyara::network::presence(){
    while(1){
        _beacon.broadcast();
        _link.routes().depreciate([this](const dhyara::routing::route& route, dhyara::delay_type delay) mutable{
            _synchronizer.queue(route.dst(), route.via(), delay);
            vTaskDelay(pdMS_TO_TICKS(1));
        });
        vTaskDelay(pdMS_TO_TICKS(dhyara::beacon_interval));
    }
}

void dhyara::network::sync(){
    _synchronizer.run();
}

void dhyara::network::start(){
    _link.init();
    _link[dhyara::packets::type::beacon]            = _beacon;
    _link[dhyara::packets::type::acknowledgement]   = _acknowledgement;
    _link[dhyara::packets::type::advertisement]     = _advertisement;
    _link[dhyara::packets::type::echo_request]      = _echo_request;
    _link[dhyara::packets::type::echo_reply]        = _echo_reply;
    _link[dhyara::packets::type::echo_lost]         = _echo_lost;
    _link[dhyara::packets::type::chunk]             = _chunk;
    _link[dhyara::packets::type::delivered]         = _delivered;
    
    xTaskCreate(&dhyara::network::task_presence,    "presence",    3072,  this,   0,  NULL);
    xTaskCreate(&dhyara::network::task_synchronize, "synchronize", 8192,  this,   8,  NULL);
    xTaskCreate(&dhyara::network::task_start_rcvd,  "start_rcvd",  8192,  &_link, 22, NULL);
#if DHYARA_ENABLED_SEND_QUEUEING
    xTaskCreate(&dhyara::network::task_start_send,  "start_send",  2028,  &_link, 22, NULL);
#endif
}

bool dhyara::network::send(const dhyara::peer_address& target, const dhyara::packets::data& data){
    std::uint8_t chunks = data.chunks();
    bool success = true;
    dhyara::packets::data data_packet(data);
    if(data_packet.source().is_null()){
        data_packet._source = _link.address();
    }
    for(std::uint8_t c = 0; c != chunks; ++c){
        dhyara::packets::chunk chunk = data_packet.prepare(c);
        bool res = _link.send(target, dhyara::packets::type::chunk, chunk);
        success = success && res;
    }
    return success;
}

void dhyara::network::on_data(dhyara::actions::chunk::data_callback_type slot){
    _chunk.on_data(slot);
}

void dhyara::network::task_presence(void* arg){
    dhyara::network* network = reinterpret_cast<dhyara::network*>(arg);
    network->presence();
}

void dhyara::network::task_synchronize(void* arg){
    dhyara::network* network = reinterpret_cast<dhyara::network*>(arg);
    network->sync();
}

void dhyara::network::task_start_rcvd(void* arg){
    dhyara::link* link = reinterpret_cast<dhyara::link*>(arg);
    link->start_rcv();
}

#if DHYARA_ENABLED_SEND_QUEUEING

void dhyara::network::task_start_send(void* arg){
    dhyara::link* link = reinterpret_cast<dhyara::link*>(arg);
    link->start_snd();
}

#endif
