/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/tools/ping.h"
#include <inttypes.h>

dhyara::tools::ping::ping(dhyara::network& network, std::uint8_t count, std::int8_t batch, std::uint8_t sleep): _network(network), _count(count), _batch(batch), _sleep(sleep) {
    using namespace std::placeholders;
    _conn_reply = _network.echo_reply().connect(std::bind(&dhyara::tools::ping::reply, this, _1, _2));
}

void dhyara::tools::ping::operator()(const dhyara::address& addr){
    ESP_LOGI("ping", "%s count:%d batch:%d sleep:%d", addr.to_string().c_str(), _count, _batch, _sleep);
    
    reset();
    _network.echo_request().ping(addr, _count, _batch, _sleep);
    ESP_LOGI("ping", "waiting 1s for all replies");
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    std::size_t echo_sent  = _network.link().tx(dhyara::packets::type::echo_request);
    std::size_t bytes_sent = echo_sent * 250;
    std::size_t echo_rcvd  = _network.link().rx(dhyara::packets::type::echo_reply);
    std::size_t bytes_rcvd = echo_rcvd * 250;
    double echo_loss       = (double)(echo_sent - std::min(echo_rcvd, echo_sent)) / (double)echo_sent;
    dhyara::delay_type ott = _duration / 2; // one trip time assuming link is symmetric
    
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

    ESP_LOGI(
        "ping", 
        "%zu/%zu bytes in %.2lf ms (%.2lf kB/s up %.2lf kB/s dn)", 
        bytes_rcvd,
        bytes_sent,
        _duration/1000.0,
        (double)(bytes_sent*1000) / ott,
        (double)(bytes_rcvd*1000) / ott
    );
}

void dhyara::tools::ping::operator()(const std::string& addr){
    operator()(dhyara::address(addr));
}

void dhyara::tools::ping::reply(const dhyara::address&, const dhyara::packets::echo_reply& reply){
    _duration = reply.latency();
}

void dhyara::tools::ping::reset(){
    _network.link().reset(dhyara::packets::type::echo_request);
    _network.link().reset(dhyara::packets::type::echo_reply);
    _network.echo_reply().reset();
    
    _duration = 0;
}

dhyara::tools::ping::~ping(){
    _network.echo_reply().disconnect(_conn_reply);
    reset();
}

