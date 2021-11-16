/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/services/ping_impl.h"
#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include "dhyara/network.h"
#include "dhyara/dhyara.h"
#include "dhyara/services/stream.h"

dhyara::services::ping_impl::ping_impl(services::stream& stream, std::uint8_t count, std::int8_t batch, std::uint8_t sleep): _stream(stream), _count(count), _batch(batch), _sleep(sleep), _first(0), _last(0), _consumed(0), _quiet(false){
    using namespace std::placeholders;
    _conn_reply = dhyara_default_network().echo_reply().connect(std::bind(&dhyara::services::ping_impl::reply, this, _1, _2));
}

void dhyara::services::ping_impl::batch(const dhyara::address& addr){
    dhyara_default_network().echo_request().ping(addr, _batch);
    vTaskDelay(pdMS_TO_TICKS(_sleep*1000));
}

void dhyara::services::ping_impl::operator()(const dhyara::address& addr){
    ESP_LOGI("ping", "%s count:%d batch:%d sleep:%d", addr.to_string().c_str(), _count, _batch, _sleep);
    
    reset();
    _stats.reserve(_count*_batch);
    for(std::uint8_t i = 0; i < _count; ++i){
        _first = esp_timer_get_time();
        batch(addr);
        _consumed += (_last - _first);
    }
    
    std::size_t echo_sent  = dhyara_default_network().link().tx(dhyara::packets::type::echo_request);
    std::size_t echo_rcvd  = dhyara_default_network().link().rx(dhyara::packets::type::echo_reply);
    
    dhyara::delay_type latency_min = std::numeric_limits<dhyara::delay_type>::max(), latency_max = 0, latency_total = 0;
    double latency_avg = 0, latency_deviation = 0, latency_sd = 0;
    
    for(const auto& stat: _stats){
        const dhyara::delay_type& rtt = std::get<3>(stat);
        latency_total += rtt;
        latency_avg    = (double)latency_total / (double)dhyara_default_network().link().rx(dhyara::packets::type::echo_reply);
        latency_min    = std::min(latency_min, rtt);
        latency_max    = std::max(latency_max, rtt);
    }
    for(const auto& stat: _stats){
        const dhyara::delay_type& rtt = std::get<3>(stat);
        latency_deviation = std::pow(rtt - latency_avg, 2);
    }
    latency_sd = std::sqrt( latency_deviation / _stats.size());
    
    double echo_loss = (double)(echo_sent - std::min(echo_rcvd, echo_sent)) / (double)echo_sent;
    double duration = (double)_consumed/1000.0;
    
    _stream << "\n";

    _stream << "ping "
            << echo_rcvd << "/" << echo_sent << " received, " 
            << std::setprecision(2) << std::fixed << (echo_loss*100.0) << " lost, "
            << "rtt min/avg/max/total/sd = "
                << std::setprecision(2) << std::fixed << ((double)latency_min  /1000.0) << "/"
                << std::setprecision(2) << std::fixed << ((double)latency_avg  /1000.0) << "/"
                << std::setprecision(2) << std::fixed << ((double)latency_max  /1000.0) << "/"
                << std::setprecision(2) << std::fixed << ((double)latency_total/1000.0) << " ms/"
                << std::setprecision(2) << std::fixed << latency_sd
            << "\n";

    _stream << "ping "
            << echo_rcvd*250 << "/" << echo_sent*250 << " bytes " 
            << "in " << std::setprecision(2) << std::fixed << duration << "ms "
            << "(" 
                    << std::setprecision(2) << std::fixed 
                    << (((double)((echo_sent+echo_rcvd)*250*1000) / duration)/1000.0) 
                    << " kB/s" 
            << ")"
            << "\n";
    _stream << "------------------------------------------------------------------------------" << "\n";
    _stream << " N | Origin              | Hit                 | Return              | RTT          " << "\n";
    for(std::uint8_t i = 0; i < _stats.size(); ++i){
        const auto& stat = _stats.at(i);
        const auto& stat_last = (i % _batch == 0) ? stat : _stats.at(i-1);
        _stream     << std::setfill(' ') << std::setw(2) << (int)i 
                              << " |" << std::setfill(' ') << std::setw(11) << std::get<0>(stat) << "(" << std::setfill(' ') << std::setw(5) << (std::get<0>(stat) - std::get<0>(stat_last)) << "us)"
                              << " |" << std::setfill(' ') << std::setw(11) << std::get<1>(stat) << "(" << std::setfill(' ') << std::setw(5) << (std::get<1>(stat) - std::get<1>(stat_last)) << "us)"
                              << " |" << std::setfill(' ') << std::setw(11) << std::get<2>(stat) << "(" << std::setfill(' ') << std::setw(5) << (std::get<2>(stat) - std::get<2>(stat_last)) << "us)"
                              << " |" << std::get<3>(stat) << "us" 
                    << "\n";
    }

}

void dhyara::services::ping_impl::reply(const dhyara::address&, const dhyara::packets::echo_reply& reply){
    _last = esp_timer_get_time();
    _stats.push_back(std::make_tuple(reply.time(), reply.rtime(), _last, reply.latency()));
    if(!_quiet){
        _stream << reply << "\n";
    }
}

void dhyara::services::ping_impl::reset(){
    dhyara_default_network().link().reset(dhyara::packets::type::echo_request);
    dhyara_default_network().link().reset(dhyara::packets::type::echo_reply);
    
    _last  = 0;
    _first = 0;
    _stats.clear();
}

dhyara::services::ping_impl::~ping_impl(){
    dhyara_default_network().echo_reply().disconnect(_conn_reply);
    reset();
}