/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/tools/ping.h"
#include <inttypes.h>

dhyara::tools::ping::ping(dhyara::network& network, std::uint8_t count, std::int8_t batch, std::uint8_t sleep): _network(network), _count(count), _batch(batch), _sleep(sleep), _first(0), _last(0){
    using namespace std::placeholders;
    _conn_reply = _network.echo_reply().connect(std::bind(&dhyara::tools::ping::reply, this, _1, _2));
}

void dhyara::tools::ping::operator()(const dhyara::address& addr){
    ESP_LOGI("ping", "%s count:%d batch:%d sleep:%d", addr.to_string().c_str(), _count, _batch, _sleep);
    
    reset();
    _first = esp_timer_get_time();
    _network.echo_request().ping(addr, _count, _batch, _sleep);
    ESP_LOGI("ping", "waiting 1s for all replies");
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    std::size_t echo_sent  = _network.link().tx(dhyara::packets::type::echo_request);
    std::size_t echo_rcvd  = _network.link().rx(dhyara::packets::type::echo_reply);
    double echo_loss  = (double)(echo_sent - std::min(echo_rcvd, echo_sent)) / (double)echo_sent;
    dhyara::delay_type wastage = ((_count-1)*_sleep);
    
    ESP_LOGI(
        "ping", 
        "%zu/%zu received, %.2lf%% lost, rtt min/avg/max/total = %.2lf/%.2lf/%.2lf/%.2lf ms", 
        echo_rcvd,
        echo_sent,
        echo_loss*100.0,
        (double)_network.echo_reply().rtt().min/1000.0,
        (double)_network.echo_reply().rtt().avg/1000.0,
        (double)_network.echo_reply().rtt().max/1000.0,
        (double)_network.echo_reply().rtt().total/1000.0
    );
    double duration = (double)(_last - _first - wastage)/1000.0;
    ESP_LOGI(
        "ping", 
        "%zu/%zu bytes in %.2lf ms (%.2lf kB/s)", 
        echo_rcvd*250,
        echo_sent*250,
        duration,
        ((double)((echo_sent+echo_rcvd)*250*1000) / duration)/1000.0
    );
}

void dhyara::tools::ping::operator()(const std::string& addr){
    operator()(dhyara::address(addr));
}

void dhyara::tools::ping::reply(const dhyara::address&, const dhyara::packets::echo_reply& reply){
    _last = esp_timer_get_time();
}

void dhyara::tools::ping::reset(){
    _network.link().reset(dhyara::packets::type::echo_request);
    _network.link().reset(dhyara::packets::type::echo_reply);
    _network.echo_reply().reset();
    
    _last  = 0;
    _first = 0;
}

dhyara::tools::ping::~ping(){
    _network.echo_reply().disconnect(_conn_reply);
    reset();
}
