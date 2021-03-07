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

#ifndef DHYARA_PACKETS_ADVERTISEMENT_H
#define DHYARA_PACKETS_ADVERTISEMENT_H

#include <array>
#include <cstdint>
#include "dhyara/peer.h"
#include "dhyara/defs.h"

namespace dhyara{

namespace packets{
    
/**
 * advertisement packet sent after an acknowledgement packet is received.
 * The advertisement packet contains route information about a neighbour.
 * The route information includes one trip delay.
 */
struct advertisement{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    raw_address_type _dest;
    delay_type       _delay;
    
    inline advertisement(): _dest{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _delay(0) {}
    inline advertisement(const dhyara::peer::address& dest, delay_type delay): 
        _dest{dest.b1(), dest.b2(), dest.b3(), dest.b4(), dest.b5(), dest.b6()},
        _delay(delay){}
    inline std::size_t size() const {return sizeof(advertisement);}
    inline dhyara::peer::address dest() const { return dhyara::peer::address(_dest); }
    inline delay_type delay() const { return _delay; }
} __attribute__((packed));

}

}

#endif // DHYARA_PACKETS_ADVERTISEMENT_H
