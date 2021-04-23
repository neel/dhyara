/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/actions/advertisement.h"
#include "dhyara/synchronizer.h"
#include "dhyara/link.h"

void dhyara::actions::advertisement::operator()(const dhyara::address& addr, const dhyara::packets::advertisement& advertisement){
    dhyara::delay_type delay_first = std::numeric_limits<dhyara::delay_type>::max();
    if(_link.routes().exists(addr)){
        delay_first = _link.routes().next(addr).delay();
    }
    if(!_link.universe().exists(advertisement.dest())){
        _link.universe().add(advertisement.dest());
    }
    dhyara::delay_type delay = advertisement.delay() + delay_first;
    _synchronizer.queue(advertisement.dest(), addr, delay);
}
