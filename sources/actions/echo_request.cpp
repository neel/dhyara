/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/actions/echo_request.h"
#include "dhyara/link.h"

void dhyara::actions::echo_request::operator()(const dhyara::address& addr, const dhyara::packets::echo_request& echo_request){
    dhyara::address target = echo_request.target();
    if(target == _link.address()){
        // respond if self
        dhyara::packets::echo_reply reply(echo_request.source(), _link.address(), echo_request.seq(), echo_request.time(), echo_request.ttl());
        _link.send(echo_request.source(), dhyara::packets::type::echo_reply, reply);
    }else{
        // relay otherwise
        std::uint8_t ttl = echo_request.ttl()-1; // decrement TTL
        dhyara::packets::echo_request repeat(echo_request.target(), echo_request.source(), echo_request.seq(), echo_request.time(), ttl);
        if(ttl > 0){ // TTL didn't expire
            _link.send(target, dhyara::packets::type::echo_request, repeat);
        }else {
            dhyara::packets::echo_lost lost(echo_request.source(), _link.address(), echo_request.seq(), echo_request.time());
            _link.send(echo_request.source(), dhyara::packets::type::echo_lost, lost);
        }
    }
}

void dhyara::actions::echo_request::ping(const dhyara::address& target, std::uint8_t count, std::int8_t batch, std::uint8_t sleep){
    std::uint32_t seq = 1;
    std::uint8_t total = count +1;
    std::uint8_t counter = 0;
    while (++counter % total){
        std::int8_t b = batch;
        while(b-- > 0){
            dhyara::packets::echo_request request(target, _link.address(), seq++);
            _link.send(target, dhyara::packets::type::echo_request, request);
        }
        vTaskDelay(pdMS_TO_TICKS(sleep));
    }
}
