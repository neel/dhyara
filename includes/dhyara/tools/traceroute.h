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
 * Perform a traceroute operation
 * \ingroup tools
 */
struct traceroute{
    /**
     * Construct a traceroute
     * \param network reference to the network
     * \param target the destination address
     */
    traceroute(dhyara::network& network, const dhyara::peer_address& target);
    /**
     * release the connection with the echo_lost action
     */
    ~traceroute();
    /**
     * Run the traceroute
     */
    void operator()();

    private:
        /**
         * Send an ICMP request with the ttl specified
         * \param target the destination mac address
         * \param ttl TTL of the ICMP request which is to be sent
         */
        void run(const dhyara::peer_address& target, std::uint8_t ttl);
        /**
         * The callback to capture the lost packet and print the destination till which the previous ICMP request packet has reached. If that destination is the desired destination then stop sending another ICMP request. Otherwise send another with increased ttl value.
         */
        void lost(const dhyara::peer_address&, const dhyara::packets::echo_lost& lost);
    private:
        dhyara::network& _network;
        dhyara::peer_address _target;
        std::size_t _connection;
        std::size_t _sequence;
};
    
}
    
}

#endif // DHYARA_TOOLS_TRACEROUTE_H
