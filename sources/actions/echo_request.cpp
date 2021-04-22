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
