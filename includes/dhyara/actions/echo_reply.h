/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_ECHO_REPLY_H
#define DHYARA_ACTIONS_ECHO_REPLY_H

#include "dhyara/actions/action.h"
#include "dhyara/packets/echo_reply.h"
#include "dhyara/peer.h"
#include "dhyara/defs.h"

namespace dhyara{
    
struct link;
    
namespace actions{
    
/**
 * action to handle an incoming echo reply packet
 */
struct echo_reply: dhyara::actions::action<echo_reply, dhyara::packets::echo_reply>{
    struct rtt_stat{
        dhyara::delay_type min = std::numeric_limits<dhyara::delay_type>::max(), max = 0, total = 0;
        double avg = 0;
        
        inline void reset() { min = std::numeric_limits<dhyara::delay_type>::max(); max = 0; avg = 0.0; total = 0; }
    };
    
    inline explicit echo_reply(dhyara::link& link): _link(link){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::echo_reply& echo_reply);
    
    inline const rtt_stat& rtt() const { return _stat; }
    inline void reset() { 
        _stat.reset();
    }
    
    private:
        dhyara::link& _link;
        rtt_stat      _stat;
};
    
}
    
}


#endif // DHYARA_ACTIONS_ECHO_REPLY_H
