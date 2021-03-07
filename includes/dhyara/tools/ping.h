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

#ifndef DHYARA_TOOLS_PING_H
#define DHYARA_TOOLS_PING_H

#include "dhyara/network.h"

namespace dhyara{
    
namespace tools{
    
/**
 * ping tool sends ICMP requests in batches. Each batch contains <tt>batch</tt> number of requests. Number of batches is <tt>count</tt>. 
 * No explicit delay between two requests in the same batch. Sleep between two consecutive batches in <tt>sleep</tt> ms.
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
