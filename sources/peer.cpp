/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/peer.h"
#include "esp_timer.h"
#include <cstring>
#include <iostream>

dhyara::peer::peer(const dhyara::address& addr): _addr(addr){}

bool dhyara::peer::operator<(const dhyara::peer& other) const{
    return addr() < other.addr();
}

bool dhyara::peer::operator==(const dhyara::peer& other) const{
    return addr() == other.addr();
}

