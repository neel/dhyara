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
    traceroute(dhyara::network& network, const dhyara::address& target);
    /**
     * release the connection with the echo_lost action
     */
    ~traceroute();
    /**
     * Run the traceroute
     * \warning Do not return immediately after running traceroute. Use `vTaskDelay` to allow few seconds for receiving the reply from the other end.
     */
    void operator()();
    /**
     * Reset before starting another traceroute
     */
    void reset();
    private:
        /**
         * Send an Echo request with the ttl specified
         * \param target the destination mac address
         * \param ttl TTL of the Echo request which is to be sent
         */
        void run(const dhyara::address& target, std::uint8_t ttl);
        /**
         * The callback to capture the lost packet and print the destination till which the previous Echo request packet has reached. If that destination is the desired destination then stop sending another Echo request. Otherwise send another with increased ttl value.
         */
        void lost(const dhyara::address&, const dhyara::packets::echo_lost& lost);
        /**
         * The callback to capture the reply packet and print the destination till which the previous Echo request packet has reached. If that destination is the desired destination then stop sending another Echo request. Otherwise send another with increased ttl value.
         */
        void reply(const dhyara::address&, const dhyara::packets::echo_reply& reply);
    private:
        dhyara::network& _network;
        dhyara::address _target;
        std::size_t _conn_lost;
        std::size_t _conn_reply;
        std::size_t _sequence;
};
    
}
    
}

#endif // DHYARA_TOOLS_TRACEROUTE_H
