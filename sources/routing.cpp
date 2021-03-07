/*
 * Copyright (c) 2020, <copyright holder> <email>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "dhyara/routing.h"
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <sstream>
#include "esp_log.h"

void dhyara::routing::route_metric::update(dhyara::delay_type delay, bool sync_updated){
    _delay = delay;
    if(sync_updated){
        update();
    }
}

dhyara::routing::route::route(const dhyara::peer::address& dst, const dhyara::peer::address& via): _dst(dst), _via(via){}

bool dhyara::routing::route::operator<(const dhyara::routing::route& other) const{
    return std::tie(_dst, _via) < std::tie(other._dst, other._via);
}

bool dhyara::routing::route::operator==(const dhyara::routing::route& other) const{
    return _dst == other._dst && _via == other._via;
}

std::string dhyara::routing::route::to_string() const{
    std::stringstream stream;
    stream << *this;
    return stream.str();
}

bool dhyara::routing::next_hop::operator==(const dhyara::routing::next_hop& other) const{
    return _via == other._via && _delay == other._delay;
}


bool dhyara::routing::exists(const dhyara::routing::route& r) const{
    return _table.find(r) != _table.end();
}


bool dhyara::routing::update(const dhyara::routing::route& r, const dhyara::delay_type& d){
    auto it = _table.find(r);
    if(it != _table.end()){
        // if d is 0 then put 1 instead
        _mutex.lock();
        it->second.update(d ? d : 1);
        _mutex.unlock();
    }else{
        _table.insert(std::make_pair(r, route_metric(d, esp_timer_get_time())));
    }
    bool altered = update_next(r.dst());
//     if(altered){
//         std::cout << *this << std::endl;
//     }
    return altered;
}

bool dhyara::routing::depreciate(const dhyara::routing::route& r){
    auto it = _table.find(r);
    if(it != _table.end()){
        // if d is 0 then put 1 instead
        _mutex.lock();
        dhyara::delay_type now = esp_timer_get_time();
        dhyara::delay_type delta = now - it->second.updated();
        dhyara::delay_type max_upgradable = (std::numeric_limits<dhyara::delay_type>::max()-4) / 2;
        dhyara::delay_type delay = (delta < max_upgradable) ? (2 * (1+ it->second.delay())) : std::numeric_limits<dhyara::delay_type>::max();
        ESP_LOGW("dhyara", "route %s last updated %lldus ago doubling delay %lld to %lld", it->first.to_string().c_str(), delta, it->second.delay(), delay);
        it->second.update(delay);
        _mutex.unlock();
    }
    bool altered = update_next(r.dst());
//     if(altered){
//         std::cout << *this << std::endl;
//     }
    return altered;
}


dhyara::routing::next_hop dhyara::routing::next(const dhyara::peer::address& dst) const{
    auto it = _next.find(dst);
    if(it != _next.end()){
        return it->second;
    }else{
        return dhyara::routing::next_hop(dst, _def);
    }
}

dhyara::delay_type dhyara::routing::delay(const route& r) const{
    auto it = _table.find(r);
    if(it != _table.end()){
        return it->second.delay();
    }
    return _def;
}

std::ostream& dhyara::routing::print(std::ostream& os) const{
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


dhyara::routing::next_hop dhyara::routing::calculated_next(dhyara::peer::address dst) const{
    dhyara::routing::route begin(dst, dhyara::peer::address::null()), end(dst, dhyara::peer::address::all());
    table_type::const_iterator lower = _table.lower_bound(begin), upper = _table.upper_bound(end);
    if(!std::distance(lower, upper)){
        return dhyara::routing::next_hop(dst, _def);
    }
    table_type::const_iterator it = std::min_element(lower, upper, [](const table_type::value_type& a, const table_type::value_type& b){
        return a.second.delay() < b.second.delay();
    });
    return dhyara::routing::next_hop(it->first.via(), it->second.delay());
}


bool dhyara::routing::exists(const dhyara::peer::address& dst) const{
    dhyara::routing::route begin(dst, dhyara::peer::address::null()), end(dst, dhyara::peer::address::all());
    table_type::const_iterator lower = _table.lower_bound(begin), upper = _table.upper_bound(end);
    return std::distance(lower, upper);
}

bool dhyara::routing::update_next(dhyara::peer::address dst){
    dhyara::routing::next_hop next = calculated_next(dst);
    auto it = _next.find(dst);
    if(it != _next.end()){
        delay_type delta = (it->second.delay() > next.delay()) ? (it->second.delay() - next.delay()) : (next.delay() - it->second.delay());
        bool altered = (it->second.via() != next.via() || delta > dhyara::delay_tolerance);
        it->second = next;
        return altered;
    }else{
        _next.insert(std::make_pair(dst, next));
        return true;
    }
}

void dhyara::routing::depreciate(){
    dhyara::delay_type now = esp_timer_get_time();
    for(auto& kv: _table){
        auto route = kv.first;
        dhyara::delay_type delta = now - kv.second.updated();
        if(delta > dhyara::route_expiry){
            depreciate(route);
        }   
    }
}


std::ostream& dhyara::operator<<(std::ostream& os, const dhyara::routing::route& route){
    os << "Route< "<< route.dst() << " via " << route.via() << " >";
    return os;
}

std::ostream& dhyara::operator<<(std::ostream& os, const dhyara::routing::next_hop& hop){
    os << "Hop< " << " via " << hop.via() << ", DELAY " << hop.delay() << " >";
    return os;
}

std::ostream & dhyara::operator<<(std::ostream& os, const dhyara::routing& routing){
    routing.print(os);
    return os;
}
