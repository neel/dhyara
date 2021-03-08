/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_BEACON_H
#define DHYARA_PACKETS_BEACON_H

#include <cstdint>
#include "esp_timer.h"

namespace dhyara{
    
namespace packets{
    
/**
 * beacon packet
 * \ingroup packets
 */
struct beacon{
    std::uint64_t _time;
    
    /**
     * Construct a beacon packet with the current time
     */
    inline beacon(): _time(esp_timer_get_time()){}
    /**
     * Construct a beacon packet with the time provided
     */
    inline explicit beacon(std::uint64_t time): _time(time){}
    /**
     * Size of the beacon packet
     */
    inline std::size_t size() const {return sizeof(beacon);}
    /**
     * Time in the beacon packet
     */
    inline std::uint64_t time() const {return _time;}
} __attribute__((packed));

}

}

#endif // DHYARA_PACKETS_BEACON_H
