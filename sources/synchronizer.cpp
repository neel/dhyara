/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/synchronizer.h"
#include "dhyara/link.h"
#include "dhyara/packets.h"
#include "dhyara/routing/table.h"
#include "esp_log.h"

dhyara::synchronizer::synchronizer(dhyara::link& local): _link(local){}

void dhyara::synchronizer::queue(const dhyara::address& dest, const dhyara::address& via, dhyara::delay_type delay, std::uint8_t hops){
    queue(dhyara::candidate(dest, via, delay, hops));
}


void dhyara::synchronizer::queue(const dhyara::candidate& c){
    _fifo.en(c);
}

void dhyara::synchronizer::run(){
    _fifo.de([this](const dhyara::candidate& c) mutable{
        update(c.dest(), c.via(), c.delay(), c.hops());
    });
}


void dhyara::synchronizer::update(const dhyara::address& dest, const dhyara::address& via, dhyara::delay_type delay, std::uint8_t hops){
    bool suggestion = _link.routes().update(dhyara::routing::route(dest, via), delay, hops);
    sync(dest, suggestion);
}

void dhyara::synchronizer::sync(const dhyara::address& dest, bool suggested){
    dhyara::delay_type now = esp_timer_get_time();
    last_advertisement_map::iterator it = _last.find(dest);
    if(!suggested){
        if(it != _last.end()){
            dhyara::delay_type last_advertised  = it->second;
            dhyara::delay_type advertised_since = now - last_advertised;
            suggested = (advertised_since >= dhyara::advertisement_expiry);
        }else{
            suggested = true;
        }
    }
    if(suggested){
        dhyara::routing::next_hop next = _link.routes().next(dest);
        dhyara::packets::advertisement advertisement(dest, next.delay(), next.hops());
        if(it == _last.end()){
            _last.insert(std::make_pair(dest, now));
        } else {
            it->second = now;
        }
        if(_link.universe().exists(dest)){
            std::string peer_name = _link.universe().peer(dest).name();
            advertisement.name(peer_name);
        }
        for(auto it = _link.neighbours().begin(); it != _link.neighbours().end(); ++it){
            dhyara::neighbour* neighbour = it->second;
            if(!neighbour->addr().is_broadcast() && dest != neighbour->addr() && next.via() != neighbour->addr()){
                _link.send_local(neighbour->addr(), dhyara::packets::type::advertisement, advertisement);
            }
        }
    }
}
