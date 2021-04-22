/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
 * \ingroup actions
 */
struct beacon: dhyara::actions::action<beacon, dhyara::packets::beacon>{
    
    /**
     * A beacon action constructor requires the link in order to respond with an aacknowledgement.
     * \param link a reference to the link.
     */
    explicit beacon(dhyara::link& link);
    
    using action::operator();
    /**
     * The received beacon is processed in this function.
     * 
     * \param addr source address 
     * \param beacon the received beacon 
     */
    void operator()(const dhyara::address& addr, const dhyara::packets::beacon& beacon);
    
    /**
     * broadcast a beacon packet to the immediate neighbours.
     */
    void broadcast();
    
    /**
     * ban a peer from getting acknowledgement of its beacon.
     * 
     * \param addr address of the peer
     */
    void ban(const dhyara::address& addr);
    /**
     * Check whether a peer is banned or not.
     * 
     * \param addr peer address
     * \return true if that address is banned, false otherwise
     */
    inline bool banned(const dhyara::address& addr) const { return _banned.count(addr); }
    
    private:
        dhyara::link& _link;
        std::set<dhyara::address> _banned;
        wifi_mode_t _mode;
};

}

}

#endif // DHYARA_ACTIONS_BEACON_H
