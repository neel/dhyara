/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_TOOLS_TRACEROUTE_H
#define DHYARA_TOOLS_TRACEROUTE_H

#include "dhyara/network.h"
#include "dhyara/peer.h"

namespace dhyara{
    
namespace tools{
    
/**
 * Perform a traceroot operation
 */
struct traceroute{
    /**
     * Construct a traceroute
     * \param network reference to teh network
     */
    traceroute(dhyara::network& network);
    /**
     * Run the traceroute
     * \param target the destination mac address
     */
    void operator()(const dhyara::peer_address& target);
    private:
        /**
         * Send an ICMP request with the ttl specified
         * \param target the destination mac address
         * \param ttl TTL of the ICMP request which is to be sent
         */
        void run(const dhyara::peer_address& target, std::uint8_t ttl);
    private:
        dhyara::network& _network;
};
    
}
    
}

#endif // DHYARA_TOOLS_TRACEROUTE_H
