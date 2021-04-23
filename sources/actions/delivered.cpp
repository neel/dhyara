/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/actions/delivered.h"
#include "dhyara/link.h"

void dhyara::actions::delivered::operator()(const dhyara::address& addr, const dhyara::packets::delivered& delivered){
    if(delivered.target() != _link.address()){
        _link.send(delivered.target(), dhyara::packets::type::delivered, delivered);
        return;
    }
}
