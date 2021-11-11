/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/universe.h"

const dhyara::peer& dhyara::universe::add(const dhyara::peer& p){
    auto inserted = _peers.insert(std::make_pair(p.addr(), p));
    return inserted.first->second;
}

const dhyara::peer& dhyara::universe::add(const dhyara::address& addr){
    return add(dhyara::peer(addr));
}

const dhyara::peer& dhyara::universe::add(const std::string& addr){
    return add(dhyara::address(addr));
}

bool dhyara::universe::exists(const std::string& addr) const {
    return _peers.find(dhyara::address(addr)) != _peers.end();
}

bool dhyara::universe::exists(const dhyara::address& addr) const{
    return _peers.find(addr) != _peers.end();
}

std::size_t dhyara::universe::size() const{
    return _peers.size();
}

dhyara::peer& dhyara::universe::peer(const dhyara::address& addr){
    peer_collection_type::iterator it = _peers.find(addr);
    return it->second;
}
