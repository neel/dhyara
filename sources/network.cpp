/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
#if DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER
    ,_server(_link)
#endif    
    {}

void dhyara::network::presence(){
    while(1){
        _beacon.broadcast();
        if(dhyara::depreciation_policy == dhyara::depreciation_policies::amplify_delay){
            _link.routes().depreciate([this](const dhyara::routing::route& route, dhyara::delay_type delay, std::uint8_t hops) mutable{
                _synchronizer.queue(route.dst(), route.via(), delay, hops);
                vTaskDelay(pdMS_TO_TICKS(1));
            });
        }else if(dhyara::depreciation_policy == dhyara::depreciation_policies::remove_route){
            _link.routes().depreciate(_synchronizer);
            vTaskDelay(pdMS_TO_TICKS(dhyara::beacon_interval));
        }
    }
}

void dhyara::network::isolate(const dhyara::address& x, const dhyara::address& y){
    if(_link.address() == x) _beacon.ban(y);
    if(_link.address() == y) _beacon.ban(x);
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
    
#if DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER
    ESP_ERROR_CHECK(_server.start());
#endif
}

bool dhyara::network::send(const dhyara::address& target, const dhyara::packets::data& data){
    std::uint8_t chunks = data.chunks();
    bool success = true;
    dhyara::packets::data data_packet(data);
    if(data_packet.source().is_null()){
        data_packet.source(_link.address());
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
