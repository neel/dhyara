/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/actions/echo_reply.h"
#include "dhyara/link.h"

void dhyara::actions::echo_reply::operator()(const dhyara::address& addr, const dhyara::packets::echo_reply& echo_reply){
    dhyara::address target = echo_reply.target();
    if(target == _link.address()){
        dhyara::delay_type rtt = esp_timer_get_time() - echo_reply.time();
        _stat.total += rtt;
        _stat.avg    = (double)_stat.total / (double)_link.rx(dhyara::packets::type::echo_reply);
        _stat.min    = std::min(_stat.min, rtt);
        _stat.max    = std::max(_stat.max, rtt);
    }else{
        _link.send(target, dhyara::packets::type::echo_reply, echo_reply);
    }
}
