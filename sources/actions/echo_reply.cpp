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
    if(echo_reply.target() != _link.address()){
        _link.send(echo_reply.target(), dhyara::packets::type::echo_reply, echo_reply);
    }
}
