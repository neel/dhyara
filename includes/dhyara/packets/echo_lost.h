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
