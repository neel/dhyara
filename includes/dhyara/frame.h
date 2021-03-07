/*
 * Copyright (c) 2020, <copyright holder> <email>
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

#ifndef DHYARA_FRAME_H
#define DHYARA_FRAME_H

#include <iostream>
#include "dhyara/packets/types.h"
#include "dhyara/packets/io.h"

namespace dhyara{

/**
 * @todo write docs
 */
struct frame{
    typedef std::uint8_t size_type;
    
    packets::type _type;
    size_type     _length;
    std::uint8_t  _buffer[248];
    
    inline frame(): _type(packets::type::unknown){
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    inline frame(packets::type type): _type(type){
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    inline frame(packets::type type, std::size_t len): _type(type), _length(len){
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    
    constexpr static std::uint8_t header_size() { return sizeof(packets::type) + sizeof(size_type); }
    
    inline packets::type type() const {return _type;}
    inline std::size_t length() const {return _length;}
    inline std::size_t size() const {return length() + sizeof(packets::type) + sizeof(size_type);}
    
    inline void clear(){
        _type = packets::type::unknown;
        _length = 0;
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    
    template <typename PacketT>
    static frame create(packets::type type, const PacketT& packet){
        frame f(type, packet.size());
        dhyara::write(packet, f._buffer);
        return f;
    }
    template <typename PacketT>
    static frame* create_new(packets::type type, const PacketT& packet){
        frame* f = new frame(type, packet.size());
        dhyara::write(packet, f->_buffer);
        return f;
    }
} __attribute__((packed));

}

#endif // DHYARA_FRAME_H
