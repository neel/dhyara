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
 */
struct echo_lost: dhyara::actions::action<echo_lost, dhyara::packets::echo_lost>{    
    typedef std::function<void (const dhyara::packets::echo_lost&)> lost_callback_type;
    
    inline explicit echo_lost(dhyara::link& link): _link(link){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::echo_lost& echo_lost);
    
    /**
     * set a callback which will be called once a ICMP lost packet is received
     */
    inline void on_slot(lost_callback_type slot) { _slot = slot; }
    
    private:
        dhyara::link& _link;
        lost_callback_type _slot;
};
    
}
    
}

#endif // DHYARA_ACTIONS_ECHO_LOST_H
