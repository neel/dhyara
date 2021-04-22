/*
 * Copyright (c) 2021, <copyright holder> <email>
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

#include "dhyara/synchronizer.h"
#include "dhyara/link.h"
#include "dhyara/packets.h"
#include "dhyara/routing.h"
#include "esp_log.h"

dhyara::synchronizer::synchronizer(dhyara::link& local): _link(local){}

void dhyara::synchronizer::queue(const dhyara::address& dest, const dhyara::address& via, dhyara::delay_type delay){
    queue(dhyara::candidate(dest, via, delay));
}


void dhyara::synchronizer::queue(const dhyara::candidate& c){
    _fifo.en(c);
}

void dhyara::synchronizer::run(){
    _fifo.de([this](const dhyara::candidate& c) mutable{
        sync(c.dest(), c.via(), c.delay());
    });
}


void dhyara::synchronizer::sync(const dhyara::address& dest, const dhyara::address& via, dhyara::delay_type delay){
    bool sync_required = _link.routes().update(dhyara::routing::route(dest, via), delay);
    
    dhyara::delay_type now = esp_timer_get_time();
    last_advertisement_map::iterator it = _last.find(dest);
    if(!sync_required){
        if(it != _last.end()){
            dhyara::delay_type last_advertised  = it->second;
            dhyara::delay_type advertised_since = now - last_advertised;
            sync_required = (advertised_since >= dhyara::advertisement_expiry);
        }else{
            sync_required = true;
        }
    }
    if(sync_required){
        dhyara::routing::next_hop next = _link.routes().next(dest);
        dhyara::packets::advertisement advertisement(dest, next.delay());
        if(it == _last.end()){
            _last.insert(std::make_pair(dest, now));
        } else {
            it->second = now;
        }
        for(auto it = _link.neighbours().begin(); it != _link.neighbours().end(); ++it){
            const dhyara::peer& p = it->second;
            if(!p.addr().is_broadcast() && dest != p.addr()){
                _link.send_local(p.addr(), dhyara::packets::type::advertisement, advertisement);
            }
        }
    }
}
