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
    
    dhyara::peer::address _target;
    dhyara::peer::address _source;
    std::uint8_t          _packet;
    dynamic_buffer_type   _buffer;
    
    static std::uint8_t counter;
    
    /**
     * Construct a data packet
     * \note an auto incremented number is used as packet id
     * \param target target address
     * \param source source address
     */
    inline data(const dhyara::peer::address& target, const dhyara::peer::address& source): _target(target), _source(source), _packet(counter++) {}
    /**
     * Construct a data packet
     * \param target target address
     * \param packet packet id
     */
    inline data(const dhyara::peer::address& target, std::uint8_t packet): _target(target), _source(dhyara::peer::address::null()), _packet(packet) {}
    /**
     * Construct a data packet
     * \note an auto incremented number is used as packet id
     * \param target target address
     */
    inline data(const dhyara::peer::address& target): _target(target), _source(dhyara::peer::address::null()), _packet(counter++) {}
    /**
     * Construct a data packet
     * \param target target address
     * \param source source address
     * \param packet packet id
     */
    inline data(const dhyara::peer::address& target, const dhyara::peer::address& source, std::uint8_t packet): _target(target), _source(source), _packet(packet) {}
    /**
     * Construct a data packet by iterating over 
     * \note an auto incremented number is used as packet id
     * \param target target address
     * \param begin begin iterator
     * \param end end iterator
     */
    template <typename InputIt>
    inline data(const dhyara::peer::address& target, InputIt begin, InputIt end): _target(target), _source(dhyara::peer::address::null()), _packet(counter++) {
        fill(begin, end);
    }
    /**
     * target of the data packet
     */
    inline dhyara::peer::address target() const { return dhyara::peer::address(_target); }
    /**
     * source of the data packet
     */
    inline dhyara::peer::address source() const { return dhyara::peer::address(_source); }
    /**
     * packet id of data packet
     */
    inline std::uint8_t id() const { return _packet; }
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
    void copy(OutputIt it) const{
        std::copy(_buffer.begin(), _buffer.end(), it);
    }
    /**
     * appends a chunk with the data packet
     */
    void add(const dhyara::packets::chunk& chunk);
    void reserve(std::uint32_t size);
};
    
}
    
}

#endif // DHYARA_PACKETS_DATA_H
