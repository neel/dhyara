/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_ECHO_LOST_H
#define DHYARA_PACKETS_ECHO_LOST_H

#include <array>
#include <cstdint>
#include "dhyara/peer.h"
#include "esp_timer.h"

namespace dhyara{
    
namespace packets{
    

/**
 * An echo lost packet
 */
struct echo_lost{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    raw_address_type _target;
    raw_address_type _source;
    std::uint32_t    _seq;
    std::uint64_t    _time;
    
    inline echo_lost(): 
        _target{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 
        _source{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, 
        _seq(0), _time(esp_timer_get_time()){}
    inline echo_lost(const dhyara::peer::address& target, const dhyara::peer::address& source, std::uint32_t seq = 0, std::uint64_t time = esp_timer_get_time()) 
        : _target{target.b1(), target.b2(), target.b3(), target.b4(), target.b5(), target.b6()}, 
          _source{source.b1(), source.b2(), source.b3(), source.b4(), source.b5(), source.b6()}, 
          _seq(seq), _time(time){}
   
    inline std::size_t size() const {return sizeof(echo_lost); }
    inline std::uint64_t time() const {return _time;}
    inline std::uint32_t seq() const {return _seq;}
    inline dhyara::peer::address target() const { return dhyara::peer::address(_target); }
    inline dhyara::peer::address source() const { return dhyara::peer::address(_source); }
    
} __attribute__((packed));
    
}
    
}

#endif // DHYARA_PACKETS_ECHO_LOST_H
