/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_TYPES_H
#define DHYARA_PACKETS_TYPES_H

#include <cstdint>

namespace dhyara{

namespace packets{
    
/**
 * Packets types 
 * \ingroup packets
 */
enum class type : std::uint8_t{
    /**
     * Unspecified packet type
     */
    unknown,
    /**
     * beacon packet 
     * \see dhyara::packets::beacon
     */
    beacon,
    /**
     * acknowledgement packet
     * \see dhyara::packets::acknowledgement
     */
    acknowledgement,
    /**
     * advertisement packet
     * \see dhyara::packets::advertisement
     */
    advertisement,
    /**
     * chunk packet
     * \see dhyara::packets::chunk
     */
    chunk,
    /**
     * delivered packet
     * \see dhyara::packets::delivered
     */
    delivered,
    /**
     * echo_request packet
     * \see dhyara::packets::echo_request
     */
    echo_request,
    /**
     * echo_reply packet
     * \see dhyara::packets::echo_reply
     */
    echo_reply,
    /**
     * echo_lost packet
     * \see dhyara::packets::echo_lost
     */
    echo_lost
};

}

}

#endif // DHYARA_PACKETS_TYPES_H
