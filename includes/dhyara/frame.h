/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
