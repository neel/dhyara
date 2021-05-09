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
 * Each node \f$X_{i}\f$ broadcasts a \ref dhyara::packets::beacon "Beacon" at a fixed interval.
 * The \ref dhyara::packets::beacon "Beacon" contains Time at \f$X_{i}\f$ (\f$T_{i}\f$) at the time of sending.
 * The \ref dhyara::packets::beacon "Beacon" is received by all \f$X_{j}\f$ in the neighbourhood.
 * The neighbours \f$\forall X_{j} \in Neibourhood(X_{i})\f$ respond with an \ref dhyara::packets::acknowledgement "Acknowledgement", which contains the same \f$T_{i}\f$ which was received in the \ref dhyara::packets::beacon "Beacon".
 * \f$X_{i}\f$ after receiving the \ref dhyara::packets::acknowledgement "Acknowledgement" from \f$X_{j}\f$ calculates \f$\delta_{j,0}\f$ as \f$T_{i}/2\f$ and adds that as a route in \f$R_{i}\f$.
 * \code 
 * +---- 8 bytes -----+
 * |  Time at Source  |
 * +------------------+
 * \endcode
 * \ingroup packets
 */
struct acknowledgement{
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
    
    private:
        std::uint64_t _time;
} __attribute__((packed));

}

}

#endif // DHYARA_PACKETS_ACKNOWLEDGEMENT_H
