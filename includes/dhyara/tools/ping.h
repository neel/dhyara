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
 * \ingroup tools
 */
struct ping{
    /**
     * construct 
     * 
     * \param network reference to the network.
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
    void operator()(const dhyara::peer_address& addr);
    
    /**
     * perform ping operation.
     * 
     * \param addr target address to ping
     */
    void operator()(const std::string& addr);
    
    private:
        dhyara::network& _network;
        std::uint8_t     _count;
        std::int8_t      _batch;
        std::uint8_t     _sleep;
};
    
}
}

#endif // DHYARA_TOOLS_PING_H
