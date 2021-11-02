/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_PING_IMPL_H
#define DHYARA_SERVICES_PING_IMPL_H

#include <string>
#include <tuple>
#include <cstdlib>
#include <esp_http_server.h>
#include <dhyara/address.h>
#include "esp_log.h"
#include <vector>
#include <dhyara/defs.h>
#include "dhyara/packets/echo_reply.h"

namespace dhyara{

class network;

namespace services{

class stream;

struct ping_impl{
    /**
     * construct 
     * 
     * \param network reference to the network.
     * \param count number of batches
     * \param batch batch size
     * \param sleep sleep time (in ms)
     */
    ping_impl(services::stream& stream, std::uint8_t count = 1, std::int8_t batch = 1, std::uint8_t sleep = 1);
    inline void low_io(bool flag) { _low_io = flag; }
    /**
     * set number of batches
     * \param c number of batches
     */
    inline ping_impl& count(std::uint8_t c) { _count = c; return *this; }
    /**
     * set batch size
     * \param b batch size
     */
    inline ping_impl& batch(std::int8_t b) { _batch = b; return *this; }
    /**
     * set sleep (in ms)
     * \param s sleep time (ms)
     */
    inline ping_impl& sleep(std::uint8_t s) { _sleep = s; return *this; }
    
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
     * Reset before starting another traceroute
     */
    void reset();
    
    ~ping_impl();
    
    private:
        void batch(const dhyara::address& addr);
        /**
         * The callback to capture the reply packet and print the destination till which the previous Echo request packet has reached. If that destination is the desired destination then stop sending another Echo request. Otherwise send another with increased ttl value.
         */
        void reply(const dhyara::address& addr, const dhyara::packets::echo_reply& reply);
    
    private:
        using stat_tuple = std::tuple<delay_type, delay_type, delay_type, delay_type>;
        
        services::stream& _stream;
        std::uint8_t     _count;
        std::int8_t      _batch;
        std::uint8_t     _sleep;
        std::size_t      _conn_reply;
        delay_type       _first;
        delay_type       _last;
        delay_type       _consumed;
        bool             _low_io;
        
        std::vector<stat_tuple> _stats;
};


}
}

#endif // DHYARA_SERVICES_PING_IMPL_H