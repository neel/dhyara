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
 */
struct echo_request{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    raw_address_type _target;
    raw_address_type _source;
    std::uint32_t    _seq;
    std::uint64_t    _time;
    std::uint8_t     _ttl;
    
    inline echo_request(): 
        _target{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 
        _source{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 
        _seq(0), _time(esp_timer_get_time()), _ttl(255){}
    inline echo_request(const dhyara::peer::address& target, const dhyara::peer::address& source, std::uint32_t seq = 0, std::uint64_t time = esp_timer_get_time(), std::uint8_t ttl = 255) 
        : _target{target.b1(), target.b2(), target.b3(), target.b4(), target.b5(), target.b6()}, 
          _source{source.b1(), source.b2(), source.b3(), source.b4(), source.b5(), source.b6()}, 
          _seq(seq), _time(time), _ttl(ttl){}
    
    inline std::size_t size() const {return sizeof(echo_request); }
    inline std::uint64_t time() const {return _time;}
    inline std::uint32_t seq() const {return _seq;}
    inline std::uint8_t ttl() const {return _ttl;}
    inline dhyara::peer::address target() const { return dhyara::peer::address(_target); }
    inline dhyara::peer::address source() const { return dhyara::peer::address(_source); }
    
} __attribute__((packed));
    
}
    
}

#endif // DHYARA_PACKETS_ECHO_REQUEST_H
