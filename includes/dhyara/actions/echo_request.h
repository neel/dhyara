/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
 * \ingroup actions
 */
struct echo_request: dhyara::actions::action<echo_request, dhyara::packets::echo_request>{
    
    /**
     * The echo_request action is constructed using a reference to the link, so that it can respond with an echo_reply using the link if necessary
     */
    inline explicit echo_request(dhyara::link& link): _link(link){}
    
    using action::operator();
    /**
     * Process an incomming echo_request packet
     * \param addr source address 
     * \param echo_request the received echo_request 
     */
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::echo_request& echo_request);
    /**
     * Sends batch ICMP request to target. ICMP requests in the batch are sent without any time gap between two consecutive requests. The duration of sleep between two consecutive batches can be controlled using sleep argument.
     * 
     * \param target the target mac address
     * \param count number of ICMP batches to be sent (default 254)
     * \param batch number of ICMP requests in the one batch (default 1)
     * \param sleep time (in ms) to sleep after each ICMP batch request (default to 15)
     */
    void ping(const dhyara::peer_address& target, std::uint8_t count = 254, std::int8_t batch = 1, std::uint8_t sleep = 15);
    
    private:
        dhyara::link& _link;
};

}
    
}

#endif // DHYARA_ACTIONS_ECHO_REQUEST_H
