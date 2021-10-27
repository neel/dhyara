/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_ECHO_REQUEST_H
#define DHYARA_PACKETS_ECHO_REQUEST_H

#include <cstdint>
#include <array>
#include "dhyara/peer.h"
#include "esp_timer.h"

namespace dhyara{
    
namespace packets{
    
/**
 * An echo request
 * \ingroup packets
 */
struct echo_request{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    /**
     * Construct an echo request to null destination originating from null destination on current time with default ttl value
     */
    inline echo_request(): 
        _target{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 
        _source{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 
        _seq(0), _time(esp_timer_get_time()), _ttl(255){}
        
    /**
     * Construct an echo request to the target originating from the source with the provided sequence number, time and ttl 
     * \param target the target mac address
     * \param source the originating mac address
     * \param seq the sequence number of this Echo request
     * \param time the time in the Echo request (defaults to current time)
     * \param ttl TTL value in the Echo request
     */
    inline echo_request(const dhyara::address& target, const dhyara::address& source, std::uint32_t seq = 0, std::uint64_t time = 0, std::uint8_t ttl = 255) 
        : _target{target.b1(), target.b2(), target.b3(), target.b4(), target.b5(), target.b6()}, 
          _source{source.b1(), source.b2(), source.b3(), source.b4(), source.b5(), source.b6()}, 
          _seq(seq), _time(time ? time : esp_timer_get_time()), _ttl(ttl){}
    
    /**
     * Size of the packet
     */
    inline std::size_t size() const {return sizeof(echo_request); }
    /**
     * Time in the packet
     */
    inline std::uint64_t time() const {return _time;}
    /**
     * Sequence number 
     */
    inline std::uint32_t seq() const {return _seq;}
    /**
     * Sequence number 
     */
    inline void seq(std::uint32_t s) { _seq = s;}
    /**
     * TTL  
     */
    inline std::uint8_t ttl() const {return _ttl;}
    /**
     * Target address 
     */
    inline dhyara::address target() const { return dhyara::address(_target); }
    /**
     * Source address
     */
    inline dhyara::address source() const { return dhyara::address(_source); }
    
    private:
        raw_address_type _target;
        raw_address_type _source;
        std::uint32_t    _seq;
        std::uint64_t    _time;
        std::uint8_t     _ttl;
} __attribute__((packed));
    
}
    
}

#endif // DHYARA_PACKETS_ECHO_REQUEST_H
