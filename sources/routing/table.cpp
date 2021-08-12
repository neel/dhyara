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
#include "dhyara/routing/next_hop.h"

bool dhyara::routing::table::exists(const dhyara::routing::route& r) const{
    return _table.find(r) != _table.end();
}


bool dhyara::routing::table::update(const dhyara::routing::route& r, const dhyara::delay_type& d, std::uint8_t hops){
    auto it = _table.find(r);
    if(it != _table.end()){
        // if d is 0 then put 1 instead
        _mutex.lock();
        it->second.update(d ? d : 1, hops);
        _mutex.unlock();
    }else{
        _table.insert(std::make_pair(r, metric(d, hops, esp_timer_get_time())));
    }
    return update_next(r.dst());
}

bool dhyara::routing::table::depreciate(const dhyara::routing::route& r){
    constexpr static const dhyara::delay_type max = std::numeric_limits<dhyara::delay_type>::max();
    constexpr static const dhyara::delay_type max_upgradable = max / dhyara::depreciation_coefficient;
    auto it = _table.find(r);
    if(it != _table.end()){
        _mutex.lock();
        dhyara::delay_type now = esp_timer_get_time();
        dhyara::delay_type delta = now - it->second.updated();
        dhyara::delay_type current = it->second.delay();
        dhyara::delay_type delay = (current < max_upgradable) ? (dhyara::depreciation_coefficient * current) : max;
        ESP_LOGW("dhyara", "route %s last updated %" PRIu64 "us ago doubling delay %" PRIu64 " to %" PRIu64, it->first.to_string().c_str(), delta, current, delay);
        it->second.update(delay);
        _mutex.unlock();
    }
    return update_next(r.dst());
}


dhyara::routing::next_hop dhyara::routing::table::next(const dhyara::address& dst) const{
    auto it = _next.find(dst);
    if(it != _next.end()){
        return it->second;
    }else{
        return dhyara::routing::next_hop(dst, _def, 0);
    }
}

std::ostream& dhyara::routing::table::print(std::ostream& os) const{
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
    auto it = _table.find(r);
    if(it != _table.end()){
        return it->second.delay();
    }
    return _def;
}

std::uint8_t dhyara::routing::table::hops(const route& r) const{
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
        return a.second.delay() < b.second.delay();
    });
    return dhyara::routing::next_hop(it->first.via(), it->second.delay(), it->second.hops());
}


bool dhyara::routing::table::exists(const dhyara::address& dst) const{
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

void dhyara::routing::table::depreciate(){
    dhyara::delay_type now = esp_timer_get_time();
    for(auto& kv: _table){
        auto route = kv.first;
        dhyara::delay_type delta = now - kv.second.updated();
        if(delta > dhyara::route_expiry){
            depreciate(route);
        }   
    }
}

void dhyara::routing::table::depreciate(std::function<void (const dhyara::routing::route&, dhyara::delay_type, std::uint8_t)> notify){
    dhyara::delay_type now = esp_timer_get_time();
    std::vector<std::tuple<dhyara::routing::route, delay_type, std::uint8_t>> inactives;
    for(auto& kv: _table){
        auto route = kv.first;
        dhyara::delay_type delta = now - kv.second.updated();
        if(delta > dhyara::route_expiry){
            if(depreciate(route)){
                inactives.push_back(std::make_tuple(route, delay(route), hops(route)));
            }
        }   
    }
    for(const auto& node: inactives){
        notify(std::get<0>(node), std::get<1>(node), std::get<2>(node));
    }
}

dhyara::delay_type dhyara::routing::table::lost_since() const{
    if(_table.empty()) return 0;
    return std::max_element(_table.begin(), _table.end(), [](const table_type::value_type& left, const table_type::value_type& right){
        return left.second.updated() < right.second.updated();
    })->second.updated();
}

std::ostream & dhyara::routing::operator<<(std::ostream& os, const dhyara::routing::table& routing){
    routing.print(os);
    return os;
}
