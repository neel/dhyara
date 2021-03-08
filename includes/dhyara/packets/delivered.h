/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_DELIVERED_H
#define DHYARA_PACKETS_DELIVERED_H

#include <array>
#include <cstdint>
#include "dhyara/peer.h"

namespace dhyara{
    
namespace packets{
    
/**
 * Delivery report for a complete data packet, only sent if all of its chunks are successfully delivered.
 * \ingroup packets
 */
struct delivered{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    raw_address_type _target;
    raw_address_type _source;
    std::uint8_t     _packet;
    
    inline delivered(): _target{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _source{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _packet(0) {}
    inline delivered(const dhyara::peer::address& target, const dhyara::peer::address& source, std::uint8_t packet): 
        _target{target.b1(), target.b2(), target.b3(), target.b4(), target.b5(), target.b6()}, 
        _source{source.b1(), source.b2(), source.b3(), source.b4(), source.b5(), source.b6()}, 
        _packet(packet) {}
        
    inline std::size_t size() const {return sizeof(delivered); }
    inline dhyara::peer::address target() const { return dhyara::peer::address(_target); }
    inline dhyara::peer::address source() const { return dhyara::peer::address(_source); }
    inline std::uint8_t packet() const { return _packet; }
} __attribute__((packed));
    
}
    
}

#endif // DHYARA_PACKETS_DELIVERED_H
