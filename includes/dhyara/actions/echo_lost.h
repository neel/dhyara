/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_ECHO_LOST_H
#define DHYARA_ACTIONS_ECHO_LOST_H

#include "dhyara/actions/action.h"
#include "dhyara/packets/echo_lost.h"
#include <functional>

namespace dhyara{
    
struct link;
    
namespace actions{
    
/**
 * action to handle an incoming echo lost packet
 * \ingroup actions
 */
struct echo_lost: dhyara::actions::action<echo_lost, dhyara::packets::echo_lost>{    
    typedef std::function<void (const dhyara::packets::echo_lost&)> lost_callback_type;
    
    /**
     * An echo lost action is constructed using a reference to the link. The link is used for relaying the lost packet to the intended destination through the ad hoc network.
     * \param link a reference to the link
     */
    inline explicit echo_lost(dhyara::link& link): _link(link){}
    
    using action::operator();
    /**
     * Process an incomming echo_lost packet, relay if necessary.
     * \param addr source address 
     * \param echo_lost the received echo_lost 
     */
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::echo_lost& echo_lost);
    
    private:
        dhyara::link& _link;
        lost_callback_type _slot;
};
    
}
    
}

#endif // DHYARA_ACTIONS_ECHO_LOST_H
