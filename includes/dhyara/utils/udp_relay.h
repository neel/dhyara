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

#ifndef DHYARA_UDP_RELAY_H
#define DHYARA_UDP_RELAY_H

#include <cstdint>
#include <dhyara/peer.h>
#include <dhyara/packets/data.h>
#include <dhyara/network.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

namespace dhyara{

namespace utils{

/**
 * A UDP bridge between the source (identified by an IP address) and a sink (identified by a MAC address).
 * The UDP server listens on the port specified and relays all traffic to the sink node identified by the mac address specified. 
 * Whenever it receives a packet over UDP it relays that as a DATA packet over dhyara's L2 based network (to the mac address specified).
 * Whenever it receives a DATA packet over dhyara's L2 based network it relays that to <b>all</b> udp clients connected to it.
 */
class udp_relay{    
    dhyara::network&        _network;
    dhyara::address         _sink;
    std::uint16_t           _port;
    struct sockaddr_in      _local;
    struct sockaddr_in      _remote;
    int                     _socket;
    
    public:
        /**
         * Start UDP relay server
         * 
         * \param network Dhyara network
         * \param port the UDP port to listen on and send to
         */
        udp_relay(dhyara::network& network, std::uint16_t port);
        /**
         * Start UDP relay server
         * 
         * \param network Dhyara network
         * \param target mac address of the ESP32 sink node
         * \param port the UDP port to listen on and send to
         */
        udp_relay(dhyara::network& network, const dhyara::address& target, std::uint16_t port);
        
        /**
         * Run the relay and never return.
         */
        void run();
        
        /**
         * send a packet to the UDP clients
         */
        void send(const dhyara::packets::data& data);
        
        /**
         * Get the address of the ESP32 sink node
         */
        inline const dhyara::address& sink() const { return _sink; }
        /**
         * Set the address of the ESP32 sink node
         */
        inline void sink(const dhyara::address& s) { _sink = s; }
    private:
        /**
         * Initializes the relay. Initializes the local and remote endpoints and bind the local endpoint for receiving UDP packets.
         */
        int init();
        /**
         * Handle the data received from UDP 
         */
        ssize_t receive_one();
};

}

}

#endif // DHYARA_UDP_RELAY_H

