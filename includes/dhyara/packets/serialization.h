/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_SERIALIZATION_H
#define DHYARA_PACKETS_SERIALIZATION_H

#include <cstdint>
#include <functional>

namespace dhyara{

/**
 * serialize a packet
 * \ingroup packets io
 */
template <typename PacketT>
struct serialization{
    typedef PacketT packet_type;
    
    /**
     * serialize a packet and write to the output iterator
     * 
     * \param packet the packet to be serialized
     * \param output the output iterator
     */
    template <typename OutIt>
    static OutIt write(const packet_type& packet, OutIt output){
        const std::uint8_t* buffer = reinterpret_cast<const std::uint8_t*>(&packet);
        return std::copy(buffer, buffer + sizeof(packet_type), output);
    }
    
    /**
     * deserializes a packet by reading length bytes from the input iterator
     *
     * \param packet the packet to compose
     * \param input the input iterator
     * \param length length of the input
     */
    template <typename InIt>
    static InIt read(packet_type& packet, InIt input, std::size_t length){
        std::uint8_t* buffer = reinterpret_cast<std::uint8_t*>(&packet);
        InIt end = input + (length ? std::min(length, sizeof(packet_type)) : sizeof(packet_type));
        std::copy(input, end, buffer);
        return end;
    }
};
    
}

#endif // DHYARA_PACKETS_SERIALIZATION_H
