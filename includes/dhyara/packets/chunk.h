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

#ifndef DHYARA_PACKETS_CHUNK_H
#define DHYARA_PACKETS_CHUNK_H

#include <array>
#include <cstdint>
#include "dhyara/peer.h"

namespace dhyara{
    
namespace packets{
    
struct chunk{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    struct header{
        raw_address_type _target;
        raw_address_type _source;
        std::uint8_t     _length;
        std::uint8_t     _packet;
        std::uint8_t     _pending;
        
        header(): _target{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _source{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _length(0), _packet(0), _pending(0) {}
        inline header(const dhyara::peer::address& target, const dhyara::peer::address& source, std::uint8_t length, std::uint8_t packet, std::uint8_t pending = 0): 
            _target{target.b1(), target.b2(), target.b3(), target.b4(), target.b5(), target.b6()}, 
            _source{source.b1(), source.b2(), source.b3(), source.b4(), source.b5(), source.b6()}, 
            _length(length), _packet(packet), _pending(pending) {}
            
        inline std::uint8_t length() const { return _length; }
        inline std::size_t size() const {return sizeof(header); }
        inline dhyara::peer::address target() const { return dhyara::peer::address(_target); }
        inline dhyara::peer::address source() const { return dhyara::peer::address(_source); }
        inline std::uint8_t packet() const { return _packet; }
        inline std::uint8_t pending() const { return _pending; }
        inline bool is_last() const { return !pending(); }
    } __attribute__((packed));
    
    enum{capacity = 250 - (sizeof(header) +2)};
    typedef std::array<std::uint8_t, capacity> buffer_type;
    typedef buffer_type::const_iterator const_iterator_type;
    
    header           _header;
    buffer_type      _buffer;
    
    inline chunk(): _header() {
        std::fill(_buffer.begin(), _buffer.end(), 0x0);
    }
    template <typename InputIt>
    inline chunk(const dhyara::peer::address& target, const dhyara::peer::address& source, InputIt begin, std::uint8_t length, std::uint8_t packet, std::uint8_t pending = 0): _header(target, source, std::min(static_cast<std::uint8_t>(capacity), length), packet, pending) {
        InputIt end = begin + _header.length();
        std::copy(begin, end, _buffer.begin());
    }
    const header& header() const { return _header; }
    inline std::size_t size() const {return sizeof(chunk); }
    template <typename OutIt>
    OutIt copy(OutIt it){
        auto b = _buffer.begin();
        auto e = b + _header.length();
        return std::copy(b, e, it);
    }
    inline const_iterator_type begin() const { return _buffer.begin(); }
    inline const_iterator_type end() const { return begin() + _header.length(); }
    
    inline std::uint8_t length() const { return _header._length; }
    inline dhyara::peer::address target() const { return _header.target(); }
    inline dhyara::peer::address source() const { return _header.source(); }
    inline std::uint8_t packet() const { return _header.packet(); }
    inline std::uint8_t pending() const { return _header.pending(); }
    inline bool is_last() const { return !pending(); }
} __attribute__((packed));
    
}

}

#endif // DHYARA_PACKETS_CHUNK_H
