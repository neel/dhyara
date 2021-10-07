/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/tools/ping.h"
#include <inttypes.h>
#include <iostream>
#include <iomanip>

dhyara::tools::ping::ping(dhyara::network& network, std::uint8_t count, std::int8_t batch, std::uint8_t sleep): _network(network), _count(count), _batch(batch), _sleep(sleep), _first(0), _last(0){
    using namespace std::placeholders;
    _conn_reply = _network.echo_reply().connect(std::bind(&dhyara::tools::ping::reply, this, _1, _2));
}

void dhyara::tools::ping::operator()(const dhyara::address& addr){
    ESP_LOGI("ping", "%s count:%d batch:%d sleep:%d", addr.to_string().c_str(), _count, _batch, _sleep);
    
    reset();
    _stats.reserve(_count*_batch);
    _first = esp_timer_get_time();
    _network.echo_request().ping(addr, _count, _batch, _sleep);
    ESP_LOGI("ping", "waiting 1s for all replies");
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    std::size_t echo_sent  = _network.link().tx(dhyara::packets::type::echo_request);
    std::size_t echo_rcvd  = _network.link().rx(dhyara::packets::type::echo_reply);
    
    dhyara::delay_type latency_min = std::numeric_limits<dhyara::delay_type>::max(), latency_max = 0, latency_total = 0;
    double latency_avg = 0, latency_deviation = 0, latency_sd = 0;
    
    for(const auto& stat: _stats){
        const dhyara::delay_type& rtt = std::get<3>(stat);
        latency_total += rtt;
        latency_avg    = (double)latency_total / (double)_network.link().rx(dhyara::packets::type::echo_reply);
        latency_min    = std::min(latency_min, rtt);
        latency_max    = std::max(latency_max, rtt);
    }
    for(const auto& stat: _stats){
        const dhyara::delay_type& rtt = std::get<3>(stat);
        latency_deviation = std::pow(rtt - latency_avg, 2);
    }
    latency_sd = std::sqrt( latency_deviation / _stats.size());
    
    double echo_loss  = (double)(echo_sent - std::min(echo_rcvd, echo_sent)) / (double)echo_sent;
    dhyara::delay_type wastage = ((_count-1)*_sleep);
    
    ESP_LOGI(
        "ping", 
        "%zu/%zu received, %.2lf%% lost, rtt min/avg/max/total/sd = %.2lf/%.2lf/%.2lf/%.2lf ms/%.2lf", 
        echo_rcvd,
        echo_sent,
        echo_loss*100.0,
        (double)latency_min/1000.0,
        (double)latency_avg/1000.0,
        (double)latency_max/1000.0,
        (double)latency_total/1000.0,
        (double)latency_sd
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
    std::cout << "N Origin           Hit                Return           RTT            " << std::endl;
    for(std::uint8_t i = 0; i < _stats.size(); ++i){
        const auto& stat = _stats.at(i);
        const auto& stat_last = (i == 0) ? stat : _stats.at(i-1);
        std::cout   << (int)i << " " << std::get<0>(stat) << "(" << std::setfill(' ') << std::setw(4) << (std::get<0>(stat) - std::get<0>(stat_last)) << "us)"
                              << " " << std::get<1>(stat) << "(" << std::setfill(' ') << std::setw(4) << (std::get<1>(stat) - std::get<1>(stat_last)) << "us)"
                              << " " << std::get<2>(stat) << "(" << std::setfill(' ') << std::setw(4) << (std::get<2>(stat) - std::get<2>(stat_last)) << "us)"
                              << " " << std::get<3>(stat) << "us" 
                    << std::endl;
    }
}

void dhyara::tools::ping::operator()(const std::string& addr){
    operator()(dhyara::address(addr));
}

void dhyara::tools::ping::reply(const dhyara::address&, const dhyara::packets::echo_reply& reply){
    _last = esp_timer_get_time();
    _stats.push_back(std::make_tuple(reply.time(), reply.rtime(), _last, reply.latency()));
}

void dhyara::tools::ping::reset(){
    _network.link().reset(dhyara::packets::type::echo_request);
    _network.link().reset(dhyara::packets::type::echo_reply);
    
    _last  = 0;
    _first = 0;
    _stats.clear();
}

dhyara::tools::ping::~ping(){
    _network.echo_reply().disconnect(_conn_reply);
    reset();
}
