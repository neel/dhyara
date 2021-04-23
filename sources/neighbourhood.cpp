/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/neighbourhood.h"

const dhyara::neighbour& dhyara::neighbourhood::add(const dhyara::neighbour& p){
    auto inserted = _neighbours.insert(std::make_pair(p.addr(), p));
    esp_now_add_peer(p.raw());
    return inserted.first->second;
}

const dhyara::neighbour& dhyara::neighbourhood::add(const dhyara::address& addr, uint8_t ch, bool enc){
    return add(dhyara::neighbour(addr));
}

const dhyara::neighbour& dhyara::neighbourhood::add(const std::string& addr, std::uint8_t ch, bool enc){
    return add(dhyara::address(addr), ch, enc);
}

bool dhyara::neighbourhood::exists(const std::string& addr) const {
    return _neighbours.find(dhyara::address(addr)) != _neighbours.end();
}

bool dhyara::neighbourhood::exists(const dhyara::address& addr) const{
    return _neighbours.find(addr) != _neighbours.end();
}

std::size_t dhyara::neighbourhood::size() const{
    return _neighbours.size();
}


dhyara::neighbour& dhyara::neighbourhood::neighbour(const dhyara::address& addr){
    neighbour_collection_type::iterator it = _neighbours.find(addr);
    return it->second;
}
