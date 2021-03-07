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

#ifndef DHYARA_PACKETS_DATA_H
#define DHYARA_PACKETS_DATA_H

#include <string>
#include <cstdint>
#include <cmath>
#include "dhyara/peer.h"
#include "dhyara/packets/chunk.h"

namespace dhyara{
    
namespace packets{
    
struct data{
    typedef std::basic_string<std::uint8_t> dynamic_buffer_type;
    
    dhyara::peer::address _target;
    dhyara::peer::address _source;
    std::uint8_t          _packet;
    dynamic_buffer_type   _buffer;
    
    static std::uint8_t counter;
    
    inline data(const dhyara::peer::address& target, const dhyara::peer::address& source): _target(target), _source(source), _packet(counter++) {}
    inline data(const dhyara::peer::address& target, std::uint8_t packet): _target(target), _source(dhyara::peer::address::null()), _packet(packet) {}
    inline data(const dhyara::peer::address& target): _target(target), _source(dhyara::peer::address::null()), _packet(counter++) {}
    inline data(const dhyara::peer::address& target, const dhyara::peer::address& source, std::uint8_t packet): _target(target), _source(source), _packet(packet) {}
    template <typename InputIt>
    inline data(const dhyara::peer::address& target, InputIt begin, InputIt end): _target(target), _source(dhyara::peer::address::null()), _packet(counter++) {
        fill(begin, end);
    }
    inline dhyara::peer::address target() const { return dhyara::peer::address(_target); }
    inline dhyara::peer::address source() const { return dhyara::peer::address(_source); }
    inline std::uint8_t id() const { return _packet; }
    inline bool operator<(const data& other) const { return _packet < other._packet; }
    inline std::size_t length() const { return _buffer.length(); }
    inline std::uint8_t chunks() const { return std::ceil(static_cast<float>(length()) / static_cast<float>(dhyara::packets::chunk::capacity)); }
    chunk prepare(std::uint8_t n) const;
    template <typename InputIt>
    void fill(InputIt begin, InputIt end){
//         _buffer.reserve(std::distance(begin, end));
        std::copy(begin, end, std::back_inserter(_buffer));
    }
    template <typename OutputIt>
    void copy(OutputIt it) const{
        std::copy(_buffer.begin(), _buffer.end(), it);
    }
    void add(const dhyara::packets::chunk& chunk);
    void reserve(std::uint32_t size);
};
    
}
    
}

#endif // DHYARA_PACKETS_DATA_H
