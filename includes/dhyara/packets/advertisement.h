/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
 * The advertisement packet contains route information about a node (dest).
 * The route information includes one trip delay.
 * \ingroup packets
 */
struct advertisement{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    /**
     * Construct an advertisement packet with null destination and no delay
     */
    inline advertisement(): _dest{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _delay(0) {}
    /**
     * Construct an advertisement packet with the given destination and the delay specified.
     * 
     * \param dest The destination address
     * \param delay The delay encountered in the route
     */
    inline advertisement(const dhyara::address& dest, delay_type delay): 
        _dest{dest.b1(), dest.b2(), dest.b3(), dest.b4(), dest.b5(), dest.b6()},
        _delay(delay){}
    /**
     * Size of the packet
     */
    inline std::size_t size() const {return sizeof(advertisement);}
    /**
     * Destination in the advertisement packet
     */
    inline dhyara::address dest() const { return dhyara::address(_dest); }
    /**
     * Delay in the advertisement packet
     */
    inline delay_type delay() const { return _delay; }
    
    private:
        raw_address_type _dest;
        delay_type       _delay;
} __attribute__((packed));

}

}

#endif // DHYARA_PACKETS_ADVERTISEMENT_H
