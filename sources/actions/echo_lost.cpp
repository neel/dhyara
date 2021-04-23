/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/actions/echo_lost.h"
#include "dhyara/link.h"

void dhyara::actions::echo_lost::operator()(const dhyara::address& addr, const dhyara::packets::echo_lost& echo_lost){
    dhyara::address target = echo_lost.target();
    if(target == _link.address()){
        if(_slot){
            _slot(echo_lost);
        }
    }else{
        _link.send(target, dhyara::packets::type::echo_lost, echo_lost);
    }
}
