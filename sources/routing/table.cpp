/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/routing/table.h"
#include "esp_log.h"
#include <inttypes.h>
#include <mutex>
#include "dhyara/routing/next_hop.h"
#include "dhyara/synchronizer.h"

bool dhyara::routing::table::exists(const dhyara::routing::route& r) const{
    std::lock_guard<std::mutex> lock(_mutex);
    return _table.find(r) != _table.end();
}


bool dhyara::routing::table::update(const dhyara::routing::route& r, const dhyara::delay_type& d, std::uint8_t hops){
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _table.find(r);
    if(it != _table.end()){
        it->second.update(d, hops);
    }else{
        _table.insert(std::make_pair(r, metric(d, hops, esp_timer_get_time())));
    }
    return update_next(r.dst());
}

bool dhyara::routing::table::reachable(const dhyara::address& dst, const dhyara::address& via) const{
    return exists(dhyara::routing::route(dst, via));
}

dhyara::routing::next_hop dhyara::routing::table::next(const dhyara::address& dst) const{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _next.find(dst);
    if(it != _next.end()){
        return it->second;
    }else{
        return dhyara::routing::next_hop(dst, _def, 0);
    }
}

std::ostream& dhyara::routing::table::print(std::ostream& os) const{
    std::lock_guard<std::mutex> lock(_mutex);
    os << "Routing< " << std::endl;
    os << "\tTable< " << std::endl;
    for(auto kv: _table){
        os << "\t\t" << kv.first << " -> " << kv.second.delay() << std::endl;
    }
    os << "\t>" << std::endl;
    os << "\tNext< " << std::endl;
    for(auto kv: _next){
        os << "\t\t" << kv.first << " <- " << kv.second << std::endl;
    }
    os << "\t>" << std::endl;
    os << ">" << std::endl;
    return os;
}

dhyara::delay_type dhyara::routing::table::delay(const route& r) const{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _table.find(r);
    if(it != _table.end()){
        return it->second.delay();
    }
    return _def;
}

std::uint8_t dhyara::routing::table::hops(const route& r) const{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _table.find(r);
    if(it != _table.end()){
        return it->second.hops();
    }
    return 0;
}

dhyara::routing::next_hop dhyara::routing::table::calculated_next(dhyara::address dst) const{
    dhyara::routing::route begin(dst, dhyara::address::null()), end(dst, dhyara::address::all());
    table_type::const_iterator lower = _table.lower_bound(begin), upper = _table.upper_bound(end);
    if(!std::distance(lower, upper)){
        return dhyara::routing::next_hop(dst, _def, 0);
    }
    table_type::const_iterator it = std::min_element(lower, upper, [](const table_type::value_type& a, const table_type::value_type& b){
        return a.second.hops() < b.second.hops() || (a.second.hops() == b.second.hops() && a.second.delay() < b.second.delay());
    });
    return dhyara::routing::next_hop(it->first.via(), it->second.delay(), it->second.hops());
}


bool dhyara::routing::table::exists(const dhyara::address& dst) const{
    std::lock_guard<std::mutex> lock(_mutex);
    dhyara::routing::route begin(dst, dhyara::address::null()), end(dst, dhyara::address::all());
    table_type::const_iterator lower = _table.lower_bound(begin), upper = _table.upper_bound(end);
    return std::distance(lower, upper);
}

bool dhyara::routing::table::update_next(dhyara::address dst){
    dhyara::routing::next_hop next = calculated_next(dst);
    auto it = _next.find(dst);
    if(it != _next.end()){
        delay_type delta = (it->second.published() > next.delay()) ? (it->second.published() - next.delay()) : (next.delay() - it->second.published());
        it->second = next;
        if(it->second.via() != next.via() || delta > dhyara::delay_tolerance){
            it->second.publish();
            return true;
        }
        return false;
    }else{
        _next.insert(std::make_pair(dst, next));
        return true;
    }
}

void dhyara::routing::table::depreciate(dhyara::synchronizer& synchronizer){
    std::lock_guard<std::mutex> lock(_mutex);
    dhyara::delay_type now = esp_timer_get_time();
    std::set<dhyara::routing::route> inactives;
    for(auto it = _table.begin(); it != _table.end(); ++it){
        auto route = it->first;
        dhyara::delay_type delta = now - it->second.updated();
        if(delta > dhyara::route_expiry){
            inactives.insert(route);
        }
    }

    if(_table.size() == inactives.size() && _table.size() > 0){
        ESP_LOGW("dhyara", "No Routes! Isolated from the network (route_expiry %" PRId64 " us)", dhyara::route_expiry);
    }

    for(const dhyara::routing::route& r: inactives){
        _table.erase(r);
        bool suggested = update_next(r.dst());
        synchronizer.sync(r.dst(), suggested);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

dhyara::delay_type dhyara::routing::table::lost_since() const{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_table.empty()) return 0;
    return std::max_element(_table.begin(), _table.end(), [](const table_type::value_type& left, const table_type::value_type& right){
        return left.second.updated() < right.second.updated();
    })->second.updated();
}

std::ostream & dhyara::routing::operator<<(std::ostream& os, const dhyara::routing::table& routing){
    routing.print(os);
    return os;
}
