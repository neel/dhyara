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

#ifndef DHYARA_ACTIONS_ECHO_REQUEST_H
#define DHYARA_ACTIONS_ECHO_REQUEST_H

#include "dhyara/actions/action.h"
#include "dhyara/packets/echo_request.h"
#include "dhyara/peer.h"

namespace dhyara{
    
struct link;
    
namespace actions{
    
/**
 * action to handle an incoming echo request packet
 */
struct echo_request: dhyara::actions::action<echo_request, dhyara::packets::echo_request>{
    
    inline explicit echo_request(dhyara::link& link): _link(link){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::echo_request& echo_request);
    /**
     * Sends batch ICMP request to target. ICMP requests in the batch are sent without any time gap between two consecutive requests. The duration of sleep between two consecutive batches can be controlled using sleep argument.
     * 
     * \param target the target mac address
     * \param count number of ICMP batches to be sent (default 254)
     * \param batch number of ICMP requests in the one batch (default 1)
     * \param sleep time (in ms) to sleep after each ICMP batch request (default to 15)
     */
    void ping(const dhyara::peer::address& target, std::uint8_t count = 254, std::int8_t batch = 1, std::uint8_t sleep = 15);
    
    private:
        dhyara::link& _link;
};

}
    
}

#endif // DHYARA_ACTIONS_ECHO_REQUEST_H
