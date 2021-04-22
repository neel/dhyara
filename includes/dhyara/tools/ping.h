/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_TOOLS_PING_H
#define DHYARA_TOOLS_PING_H

#include "dhyara/network.h"

namespace dhyara{
    
namespace tools{
    
/**
 * ping tool sends ICMP requests in batches. Each batch contains <tt>batch</tt> number of requests. Number of batches is <tt>count</tt>. 
 * No explicit delay between two requests in the same batch. Sleep between two consecutive batches in <tt>sleep</tt> ms.
 * It reports the min, max, avg and total RTT (Round Trip Time) between the source and the target.
 * It also reports the total duration as half of the difference between the first sent time and last received time.
 * Following is an example of ping output. The target node's mac address is 4c:11:ae:9c:a6:85. tx/rx are the packets or bytes transmitted vs received.
 * In this example it took ms 12.7 for the last ICMPR to be received since the first ICMPQ was sent. 
 * According to this report 6.35 ms is the expected time, it may take for the target node to receive a 5000 byte long payload transmitted by the source node, in the current condition of wireless medium.
 * \code
 * I (41130) ping: 4c:11:ae:9c:a6:85
 * I (41140) ping: waiting 1s for all replies
 * I (42140) ping: 20/20 received, 0.00% lost, rtt min/avg/max/total = 1.07/1.24/1.78/24.85 ms
 * I (42140) ping: 5000/5000 bytes in 12.7 ms
 * \endcode
 * Following example demonstarate usage of ping tool.
 * \code
 * dhyara::address target("4c:11:ae:9c:a6:85")
 * dhyara::tools::ping ping(_network);
 * ping.count(50).batch(20).sleep(1);
 * ping(target);
 * \endcode
 * \ingroup tools
 */
struct ping{
    /**
     * construct 
     * 
     * \param network reference to the network.
     * \param count number of batches
     * \param batch batch size
     * \param sleep sleep time (in ms)
     */
    ping(dhyara::network& network, std::uint8_t count = 254, std::int8_t batch = 1, std::uint8_t sleep = 15);
    
    /**
     * set number of batches
     * \param c number of batches
     */
    inline ping& count(std::uint8_t c) { _count = c; return *this; }
    /**
     * set batch size
     * \param b batch size
     */
    inline ping& batch(std::int8_t b) { _batch = b; return *this; }
    /**
     * set sleep (in ms)
     * \param s sleep time (ms)
     */
    inline ping& sleep(std::uint8_t s) { _sleep = s; return *this; }
    
    /**
     * get number of batches
     */
    inline std::uint8_t count() const { return _count; }
    /**
     * get batch size
     */
    inline std::int8_t batch() const { return _batch; }
    /**
     * get sleep (in ms)
     */
    inline std::uint8_t sleep() const { return _sleep; }
    
    /**
     * perform ping operation.
     * 
     * \param addr target address to ping
     */
    void operator()(const dhyara::address& addr);
    
    /**
     * perform ping operation.
     * 
     * \param addr target address to ping
     */
    void operator()(const std::string& addr);

    /**
     * Reset before starting another traceroute
     */
    void reset();
    
    ~ping();
    
    private:
        /**
         * The callback to capture the reply packet and print the destination till which the previous ICMP request packet has reached. If that destination is the desired destination then stop sending another ICMP request. Otherwise send another with increased ttl value.
         */
        void reply(const dhyara::address& addr, const dhyara::packets::echo_reply& reply);
    
    private:
        dhyara::network& _network;
        std::uint8_t     _count;
        std::int8_t      _batch;
        std::uint8_t     _sleep;
        std::size_t      _conn_reply;
        delay_type       _first;
        delay_type       _last;
};
    
}
}

#endif // DHYARA_TOOLS_PING_H
