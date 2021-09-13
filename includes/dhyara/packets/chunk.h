/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_CHUNK_H
#define DHYARA_PACKETS_CHUNK_H

#include <array>
#include <cstdint>
#include "dhyara/peer.h"

namespace dhyara{
    
namespace packets{
    
/**
 * Header in the chunk
 * \ingroup packets
 */
struct chunk_header{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    /**
     * Construct a chunk header with null target and source. length, packet id, pending is set to 0
     */
    chunk_header(): _target{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _source{0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, _length(0), _packet(0), _pending(0) {}
    /**
     * Construct a chunk header with provided parameters
     * \param target target MAC address
     * \param source source MAC address
     * \param length length of the chunk
     * \param packet packet id of the data packet
     * \param pending number of chunks pending after this 
     */
    inline chunk_header(const dhyara::address& target, const dhyara::address& source, std::uint8_t length, std::uint8_t packet, std::uint8_t pending = 0): 
        _target{target.b1(), target.b2(), target.b3(), target.b4(), target.b5(), target.b6()}, 
        _source{source.b1(), source.b2(), source.b3(), source.b4(), source.b5(), source.b6()}, 
        _length(length), _packet(packet), _pending(pending) {}
        
    /**
     * length of the chunk
     */
    inline std::uint8_t length() const { return _length; }
    /**
     * byte size of the header
     */
    inline std::size_t size() const {return sizeof(chunk_header); }
    /**
     * target MAC address of the chunk
     */
    inline dhyara::address target() const { return dhyara::address(_target); }
    /**
     * source MAC address of the chunk
     */
    inline dhyara::address source() const { return dhyara::address(_source); }
    /**
     * packet id of the data packet that this chunk is part of
     */
    inline std::uint8_t packet() const { return _packet; }
    /**
     * number of pending chunks after this
     */
    inline std::uint8_t pending() const { return _pending; }
    /**
     * Whether this is the last chunk
     */
    inline bool is_last() const { return !pending(); }
    
    void length(std::uint8_t len) { _length = len; }
    void pending(std::uint8_t p) { _pending = p; }
    
    private:
        raw_address_type _target;
        raw_address_type _source;
        std::uint8_t     _length;
        std::uint8_t     _packet;
        std::uint8_t     _pending;
} __attribute__((packed));
    
/**
 * Chunk of a variable sized data
 * \ingroup packets
 */
struct chunk{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    
    enum{capacity = 250 - (sizeof(chunk_header) +2)};
    typedef std::array<std::uint8_t, capacity> buffer_type;
    typedef buffer_type::const_iterator const_iterator_type;
    typedef buffer_type::iterator iterator_type;
    
    /**
     * Construct an empty chunk
     */
    inline chunk(): _header() {
        clear();
    }
    inline void clear(){
        std::fill(_buffer.begin(), _buffer.end(), 0x0);
    }
    /**
     * Construct a chunk by reading length bytes from begin iterator.
     * \param target the target MAC address
     * \param source the source MAC address
     * \param packet packet id of the data packet of which this chunk is part of
     */
    inline chunk(const dhyara::address& target, const dhyara::address& source, std::uint8_t packet): _header(target, source, 0, packet, 0) {}
    /**
     * Construct a chunk by reading length bytes from begin iterator.
     * \param target the target MAC address
     * \param source the source MAC address
     * \param begin the begin iterator
     * \param length length of the chunk
     * \param packet packet id of the data packet of which this chunk is part of
     * \param pending number of chunks pending after this 
     */
    template <typename InputIt>
    inline chunk(const dhyara::address& target, const dhyara::address& source, InputIt begin, std::uint8_t length, std::uint8_t packet, std::uint8_t pending = 0): _header(target, source, std::min(static_cast<std::uint8_t>(capacity), length), packet, pending) {
        InputIt end = begin + _header.length();
        std::copy(begin, end, _buffer.begin());
    }
    /**
     * returns the header
     */
    const chunk_header& header() const { return _header; }
    /**
     * byte size of the chunk
     */
    inline std::size_t size() const {return sizeof(chunk); }
    /**
     * copy the contents of the chunk to the output iterator provided
     * \param it output iterator
     */
    template <typename OutIt>
    OutIt copy_to(OutIt it) const{
        auto b = _buffer.begin();
        auto e = b + _header.length();
        return std::copy(b, e, it);
    }
    
    /**
     * begin of the chunk payload
     */
    inline iterator_type begin() { return _buffer.begin(); }
    /**
     * end of the chunk payload
     */
    inline iterator_type end() { return _buffer.end(); }
    /**
     * begin of the chunk payload
     */
    inline const_iterator_type begin() const { return _buffer.begin(); }
    /**
     * end of teh chunk payload
     */
    inline const_iterator_type end() const { return begin() + _header.length(); }
    
    /**
     * length of the chunk
     */
    inline std::uint8_t length() const { return _header.length(); }
    /**
     * target of the chunk
     */
    inline dhyara::address target() const { return _header.target(); }
    /**
     * source of the chunk
     */
    inline dhyara::address source() const { return _header.source(); }
    /**
     * packet id of the chunk
     */
    inline std::uint8_t packet() const { return _header.packet(); }
    /**
     * number of chunks pending after this 
     */
    inline std::uint8_t pending() const { return _header.pending(); }
    /**
     * checks whether the chunks is the last
     */
    inline bool is_last() const { return !pending(); }
    
    void length(std::uint8_t len) { _header.length(len); }
    void pending(std::uint8_t p) { _header.pending(p); }
    
    private:
        chunk_header     _header;
        buffer_type      _buffer;
} __attribute__((packed));
    
}

}

#endif // DHYARA_PACKETS_CHUNK_H
