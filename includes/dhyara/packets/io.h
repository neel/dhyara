/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_IO_H
#define DHYARA_PACKETS_IO_H

#include "dhyara/packets/serialization.h"
#include <ostream>

namespace dhyara{
    
/**
 * write a packet
 * \ingroup io 
 */
template <typename PacketT, typename OutIt>
OutIt write(const PacketT& packet, OutIt output){
    return serialization<PacketT>::write(packet, output);
}

/**
 * read into a packet
 * \ingroup io 
 */
template <typename PacketT, typename InIt>
InIt read(PacketT& packet, InIt input, std::size_t length = 0){
    return serialization<PacketT>::read(packet, input, length);
}
    
namespace packets{

    struct beacon;
    struct acknowledgement;
    struct advertisement;
    struct chunk;
    struct delivered;
    struct echo_request;
    struct echo_reply;
    struct echo_lost;

    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::beacon& beacon);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::acknowledgement& acknowledgement);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::advertisement& advertisement);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::chunk& chunk);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::delivered& delivered);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::echo_request& echo_request);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::echo_reply& echo_reply);
    std::ostream& operator<<(std::ostream& stream, const dhyara::packets::echo_lost& echo_lost);

}

}

#endif // DHYARA_PACKETS_IO_H
