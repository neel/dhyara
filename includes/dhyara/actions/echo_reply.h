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
