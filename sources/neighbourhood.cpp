/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/neighbourhood.h"
#include "dhyara/routing/table.h"
#include "esp_log.h"

const dhyara::neighbour& dhyara::neighbourhood::add(dhyara::neighbour* p){
    std::lock_guard<std::mutex> lock(_mutex);
    auto inserted = _neighbours.insert(std::make_pair(p->addr(), p));
    esp_err_t err = esp_now_add_peer(p->raw());
    if(err != ESP_OK){
        ESP_LOGE("dhyara", "Error %s while adding peer %s", esp_err_to_name(err), p->addr().to_string().c_str());
    }
    return *(inserted.first->second);
}

const dhyara::neighbour& dhyara::neighbourhood::add(const dhyara::address& addr, uint8_t ch, bool enc){
    return add(new dhyara::neighbour(addr));
}

const dhyara::neighbour& dhyara::neighbourhood::add(const std::string& addr, std::uint8_t ch, bool enc){
    return add(dhyara::address(addr), ch, enc);
}

bool dhyara::neighbourhood::exists(const std::string& addr) const {
    return exists(dhyara::address(addr));
}

bool dhyara::neighbourhood::exists(const dhyara::address& addr) const{
    std::lock_guard<std::mutex> lock(_mutex);
    return _neighbours.find(addr) != _neighbours.end();
}

std::size_t dhyara::neighbourhood::size() const{
    std::lock_guard<std::mutex> lock(_mutex);
    return _neighbours.size();
}

dhyara::neighbour& dhyara::neighbourhood::neighbour(const dhyara::address& addr){
    std::lock_guard<std::mutex> lock(_mutex);
    neighbour_collection_type::iterator it = _neighbours.find(addr);
    return *(it->second);
}

std::size_t dhyara::neighbourhood::remove_unreachables(const dhyara::routing::table& routes){
    std::lock_guard<std::mutex> lock(_mutex);
    std::size_t removed = 0;
    for(neighbour_collection_type::iterator it = _neighbours.begin(); it != _neighbours.end(); ){
        if (!it->first.is_broadcast() && !routes.reachable(it->first, dhyara::address::null())) { // Not reachable directly
            // remove from neighbourhood
            esp_now_del_peer(it->first.raw());
            delete it->second;
            it->second = 0x0;
            it = _neighbours.erase(it);
            ++removed;
        }else{
            ++it;
        }
    }
    return removed;
}