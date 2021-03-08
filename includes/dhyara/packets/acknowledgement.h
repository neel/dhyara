/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_ACKNOWLEDGEMENT_H
#define DHYARA_PACKETS_ACKNOWLEDGEMENT_H

#include <cstdint>
#include "esp_timer.h"

namespace dhyara{
    
namespace packets{
    
/**
 * acknowledgement packet sent when a beacon packet is received.
 * The acknowledgement packet contains the same time that the beacon packet had.
 * \ingroup packets
 */
struct acknowledgement{
    std::uint64_t _time;
    
    /**
     * Construct an acknowledgement packet with current time
     */
    inline acknowledgement(): _time(esp_timer_get_time()){}
    /**
     * Construct an acknowledgement packet with teh time provided
     * \param time time 
     */
    inline explicit acknowledgement(std::uint64_t time): _time(time){}
    /**
     * Size of the packet
     */
    inline std::size_t size() const { return sizeof(acknowledgement); }
    /**
     * Time in the acknowledgement packet
     */
    inline std::uint64_t time() const { return _time; }
} __attribute__((packed));

}

}

#endif // DHYARA_PACKETS_ACKNOWLEDGEMENT_H
