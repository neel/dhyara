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

#ifndef DHYARA_PACKETS_SERIALIZATION_H
#define DHYARA_PACKETS_SERIALIZATION_H

#include <cstdint>
#include <functional>

namespace dhyara{

template <typename PacketT>
struct serialization{
    typedef PacketT packet_type;
    
    template <typename OutIt>
    static OutIt write(const packet_type& packet, OutIt output){
        const std::uint8_t* buffer = reinterpret_cast<const std::uint8_t*>(&packet);
        return std::copy(buffer, buffer + sizeof(packet_type), output);
    }
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
