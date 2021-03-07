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

#include "dhyara/tools/ping.h"

dhyara::tools::ping::ping(dhyara::network& network, std::uint8_t count, std::int8_t batch, std::uint8_t sleep): _network(network), _count(count), _batch(batch), _sleep(sleep){}

void dhyara::tools::ping::operator()(const dhyara::peer_address& addr){
    ESP_LOGI("ping", "%s", addr.to_string().c_str());
    
    _network.link().reset(dhyara::packets::type::echo_request);
    _network.link().reset(dhyara::packets::type::echo_reply);
    _network.echo_reply().reset();
    
    _network.echo_request().ping(addr, _count, _batch, _sleep);
    ESP_LOGI("dhyara", "waiting 1s for all replies");
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    std::size_t icmp_sent  = _network.link().tx(dhyara::packets::type::echo_request);
    std::size_t icmp_rcvd  = _network.link().rx(dhyara::packets::type::echo_reply);
    double icmp_loss  = (double)(icmp_sent - std::min(icmp_rcvd, icmp_sent)) / (double)icmp_sent;
    
    ESP_LOGI(
        "ping", 
        "%zu/%zu received, %.2lf%% lost, rtt min/avg/max/total = %.2lf/%.2lf/%.2lf/%.2lf ms", 
        icmp_rcvd,
        icmp_sent,
        icmp_loss*100.0,
        (double)_network.echo_reply().rtt().min/1000.0,
        (double)_network.echo_reply().rtt().avg/1000.0,
        (double)_network.echo_reply().rtt().max/1000.0,
        (double)_network.echo_reply().rtt().total/1000.0
    );
}

void dhyara::tools::ping::operator()(const std::string& addr){
    operator()(dhyara::peer_address(addr));
}

