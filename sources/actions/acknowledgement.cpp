/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/actions/acknowledgement.h"
#include "dhyara/synchronizer.h"

void dhyara::actions::acknowledgement::operator()(const dhyara::address& addr, const dhyara::packets::acknowledgement& acknowledgement){
    dhyara::delay_type now = esp_timer_get_time();    
    dhyara::delay_type rtt = (now - acknowledgement.time());

    if(!addr.is_broadcast()){
        _synchronizer.queue(addr, dhyara::address::null(), rtt/2);
    }
}
