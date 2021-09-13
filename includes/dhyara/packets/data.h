/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
    
/**
 * A virtual packet, that is never sent accros the network. Instead a data packet is splitted into multiple consecutive chunks. However after receiving all chunks of a data packet and data packet is constructed and passed to the appropriate callback.
 * \note Every data packet have a packet id, which is included in the delivery report once that data packet reaches its destination.
 * \ingroup packets
 */
struct data{
    typedef std::basic_string<std::uint8_t> dynamic_buffer_type;
    typedef typename dynamic_buffer_type::const_iterator const_iterator;
    typedef typename dynamic_buffer_type::iterator iterator;
       
    /**
     * Construct a data packet
     * \note an auto incremented number is used as packet id
     * \param target target address
     * \param source source address
     */
    inline data(const dhyara::address& target, const dhyara::address& source): _target(target), _source(source), _packet(counter++) {}
    /**
     * Construct a data packet
     * \param target target address
     * \param packet packet id
     */
    inline data(const dhyara::address& target, std::uint8_t packet): _target(target), _source(dhyara::address::null()), _packet(packet) {}
    /**
     * Construct a data packet
     * \note an auto incremented number is used as packet id
     * \param target target address
     */
    inline data(const dhyara::address& target): _target(target), _source(dhyara::address::null()), _packet(counter++) {}
    /**
     * Construct a data packet
     * \param target target address
     * \param source source address
     * \param packet packet id
     */
    inline data(const dhyara::address& target, const dhyara::address& source, std::uint8_t packet): _target(target), _source(source), _packet(packet) {}
    /**
     * Construct a data packet by iterating over 
     * \note an auto incremented number is used as packet id
     * \param target target address
     * \param begin begin iterator
     * \param end end iterator
     */
    template <typename InputIt>
    inline data(const dhyara::address& target, InputIt begin, InputIt end): _target(target), _source(dhyara::address::null()), _packet(counter++) {
        fill(begin, end);
    }
    /**
     * target of the data packet
     */
    inline const dhyara::address& target() const { return _target; }
    /**
     * source of the data packet
     */
    inline const dhyara::address& source() const { return _source; }
    /**
     * set source of the data packet
     */
    inline void source(const dhyara::address& addr) { _source = addr; }
    /**
     * packet id of data packet
     */
    inline std::uint8_t id() const { return _packet; }
    /**
     * Less than comparison operator checks packet id 
     */
    inline bool operator<(const data& other) const { return _packet < other._packet; }
    /**
     * length of the data packet
     */
    inline std::size_t length() const { return _buffer.length(); }
    /**
     * number of chunks required to send this data packet
     */
    inline std::uint8_t chunks() const { return std::ceil(static_cast<float>(length()) / static_cast<float>(dhyara::packets::chunk::capacity)); }
    /**
     * Generate the n'th chunk for this data packet
     */
    chunk prepare(std::uint8_t n) const;
    /**
     * 
     */
    void prepare(std::uint8_t n, chunk& c) const;
    /**
     * Fill the data packet by copying elements within [begin, end) range
     * \param begin begin iterator
     * \param end end iterator
     */
    template <typename InputIt>
    void fill(InputIt begin, InputIt end){
        std::copy(begin, end, std::back_inserter(_buffer));
    }
    /**
     * Copy elements of the data packet to the output iterator
     * \param it output iterator
     */
    template <typename OutputIt>
    void copy_to(OutputIt it) const{
        std::copy(_buffer.begin(), _buffer.end(), it);
    }
    /**
     * appends a chunk with the data packet
     */
    void add(const dhyara::packets::chunk& chunk);
    /**
     * reserve space in the dynamic buffer
     */
    void reserve(std::uint32_t size);
    /**
     * Returns raw data in the buffer
     */
    inline const std::uint8_t* raw() const { return _buffer.data(); }
    
    /**
     * length of the data packet
     */
    inline std::size_t length(std::uint8_t n) const {
        auto shift = std::min(static_cast<std::size_t>(n * packets::chunk::capacity), _buffer.length());
        return std::min(static_cast<std::size_t>(packets::chunk::capacity), (_buffer.length() - shift)); 
    }
    /**
     * Returns an const iterator to the data that starts from the position of n'th chunk
     */
    inline const_iterator begin(std::uint8_t n) const { 
        return _buffer.begin() + std::min(static_cast<std::size_t>(n * packets::chunk::capacity), _buffer.length()); 
    }
    /**
     * Returns an const iterator to the data that points to the ending position of n'th chunk
     */
    inline const_iterator end(std::uint8_t n) const { 
        return begin(n) + length(n); 
    }
    /**
     * Returns an const iterator to the data. The iterator dereferences to std::uint8_t
     */
    inline const_iterator begin() const { return _buffer.begin(); }
    /**
     * Returns an const iterator to the data. The iterator dereferences to std::uint8_t
     */
    inline const_iterator end() const { return _buffer.end(); }
    
    private:
        dhyara::address       _target;
        dhyara::address       _source;
        std::uint8_t          _packet;
        dynamic_buffer_type   _buffer;
        static std::uint8_t   counter;
};
    
}
    
}

#endif // DHYARA_PACKETS_DATA_H
