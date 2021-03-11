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
 * A 250 bytes frame is defined by a 2 bytes header and 248 bytes payload. The 2 bytes header insludes one byte for packet type and another byte for the length of the payload. 
 * \ingroup packets
 */
struct frame{
    typedef std::uint8_t size_type;
    
    packets::type _type;
    size_type     _length;
    std::uint8_t  _buffer[248];
    
    /**
     * Construct a frame that is supposed to contain a packet of unknown type
     */
    inline frame(): _type(packets::type::unknown){
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    /**
     * Construct a frame that is supposed to contain a packet of the given type
     * \param type type of the packet
     */
    inline frame(packets::type type): _type(type){
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    /**
     * Construct a frame of provided payload payload that is supposed to contain a packet of the given type 
     * \param type packet type
     * \param length length of the frame
     */
    inline frame(packets::type type, std::size_t len): _type(type), _length(len){
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    
    /**
     * Sise of the header of the frame (2 bytes)
     */
    constexpr static std::uint8_t header_size() { return sizeof(packets::type) + sizeof(size_type); }
    
    /**
     * Packet type in the frame
     */
    inline packets::type type() const {return _type;}
    /**
     * Length of the frame
     */
    inline std::size_t length() const {return _length;}
    /**
     * Size of the frame
     */
    inline std::size_t size() const {return length() + sizeof(packets::type) + sizeof(size_type);}
    
    /**
     * Clears the frame, zerofills the internal buffer
     */
    inline void clear(){
        _type = packets::type::unknown;
        _length = 0;
        std::fill(_buffer, _buffer + sizeof(_buffer), 0);
    }
    
    /**
     * Create a frame from a packet and a packet type
     * \return returns the created frame
     */
    template <typename PacketT>
    static frame create(packets::type type, const PacketT& packet){
        frame f(type, packet.size());
        dhyara::write(packet, f._buffer);
        return f;
    }
    /**
     * Creates a frame on heap with teh given packet and a packet type
     * \return returns pointer to the newly created frame
     */
    template <typename PacketT>
    static frame* create_new(packets::type type, const PacketT& packet){
        frame* f = new frame(type, packet.size());
        dhyara::write(packet, f->_buffer);
        return f;
    }
} __attribute__((packed));

}

#endif // DHYARA_FRAME_H
