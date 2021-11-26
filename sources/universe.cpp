/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/universe.h"
#include "dhyara/routing/table.h"

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

bool dhyara::universe::remove(const dhyara::address& addr){
    auto it = _peers.find(addr);
    if(it != _peers.end()){
        _peers.erase(it);
        return true;
    }
    return false;
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

std::size_t dhyara::universe::remove_unreachables(const dhyara::routing::table& routes){
    std::size_t removed = 0;
    for(peer_collection_type::iterator it = _peers.begin(); it != _peers.end(); ){
        if (!it->first.is_broadcast() && !routes.exists(it->first)) { // Not reachable at all
            // remove from universe
            it = _peers.erase(it);
            ++removed;
        }else{
            ++it;
        }
    }
    return removed;
}