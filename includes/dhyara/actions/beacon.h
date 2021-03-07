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

#ifndef DHYARA_ACTIONS_BEACON_H
#define DHYARA_ACTIONS_BEACON_H

#include <set>
#include "dhyara/peer.h"
#include "dhyara/actions/action.h"
#include "dhyara/packets/beacon.h"

namespace dhyara{
    
struct link;
    
namespace actions{
    
/**
 * action to handle an incoming beacon packet
 */
struct beacon: dhyara::actions::action<beacon, dhyara::packets::beacon>{
    
    inline explicit beacon(dhyara::link& link): _link(link){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::beacon& beacon);
    
    /**
     * broadcast a beacon packet to the immediate neighbours.
     */
    void broadcast();
    
    /**
     * ban a peer from getting acknowledgement of its beacon.
     * 
     * \param addr address of the peer
     */
    void ban(const dhyara::peer_address& addr);
    /**
     * Check whether a peer is banned or not.
     * 
     * \param addr peer address
     * \return true if that address is banned, false otherwise
     */
    inline bool banned(const dhyara::peer_address& addr) const { return _banned.count(addr); }
    
    private:
        dhyara::link& _link;
        std::set<dhyara::peer_address> _banned;
};

}

}

#endif // DHYARA_ACTIONS_BEACON_H
