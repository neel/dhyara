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
    unknown,
    beacon,
    acknowledgement,
    advertisement,
    chunk,
    echo_request,
    echo_reply,
    echo_lost,
    delivered
};

}

}

#endif // DHYARA_PACKETS_TYPES_H
