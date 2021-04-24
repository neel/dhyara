/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_ADVERTISEMENT_H
#define DHYARA_PACKETS_ADVERTISEMENT_H

#include <array>
#include <cstdint>
#include "dhyara/peer.h"
#include "dhyara/defs.h"
#include "dhyara/packets/serialization.h"

namespace dhyara{

namespace packets{
    
/**
 * advertisement packet sent after an acknowledgement packet is received.
 * The advertisement packet contains route information about a node (dest).
 * The route information includes one trip delay.
 * \ingroup packets
 */
struct advertisement{
    /**
     * Construct an advertisement packet with null destination and no delay
     */
    inline advertisement(): _dest(dhyara::address::null()), _delay(0) {}
    /**
     * Construct an advertisement packet with the given destination and the delay specified.
     * 
     * \param dest The destination address
     * \param delay The delay encountered in the route
     */
    inline advertisement(const dhyara::address& dest, delay_type delay, const std::string name = ""): 
        _dest(dest), _delay(delay), _name(name){}
    /**
     * Size of the packet
     */
    inline std::size_t size() const {
        return    6                             // mac address
                + sizeof(dhyara::delay_type)    // delay 
                + 1                             // name length
                + _name.size();                 // name
    }
    /**
     * Destination in the advertisement packet
     */
    inline const dhyara::address& dest() const { return _dest; }
    /**
     * Set Destination in the advertisement packet
     */
    inline void dest(const dhyara::address& d) { _dest = d; }
    /**
     * Delay in the advertisement packet
     */
    inline const delay_type& delay() const { return _delay; }
    /**
     * Delay in the advertisement packet
     */
    inline void delay(const delay_type& d) { _delay = d; }
    /**
     * name in the advertisement packet
     */
    inline const std::string& name() const{ return _name; }
    /**
     * set name in the advertisement packet
     */
    inline void name(const std::string& n) { _name = n; }
    
    private:
        dhyara::address    _dest;
        dhyara::delay_type _delay;
        std::string        _name;
};

}

/**
 * Specialization for advertisement packets
 */
template <>
struct serialization<packets::advertisement>{
    typedef std::array<std::uint8_t, 6> raw_address_type;
    /**
     * serialize a advertisement packet and write to the output iterator
     * 
     * \param packet the packet to be serialized
     * \param output the output iterator
     */
    template <typename OutIt>
    static OutIt write(const packets::advertisement& packet, OutIt output){
        std::uint8_t len = packet.name().size();
        output = std::copy_n(packet.dest().raw(), 6, output);
        output = std::copy_n(reinterpret_cast<const char*>(&packet.delay()), sizeof(dhyara::delay_type), output);
        output = std::copy_n(reinterpret_cast<const std::uint8_t*>(&len), 1, output);
        output = std::copy_n(packet.name().begin(), len, output);
        return output;
    }
    
    /**
     * deserializes a advertisement packet by reading length bytes from the input iterator
     *
     * \param packet the packet to compose
     * \param input the input iterator
     * \param length length of the input
     */
    template <typename InIt>
    static InIt read(packets::advertisement& packet, InIt input, std::size_t length){
        std::uint8_t raw_address[6];
        dhyara::delay_type delay;
        std::uint8_t len = 0;
        std::string name;
        
        if(length >= 6){
            std::copy_n(input, 6, raw_address);
            input  += 6;
            length -= 6;
            packet.dest(dhyara::address(raw_address));
        }
        if(length >= sizeof(dhyara::delay_type)){
            std::copy_n(input, sizeof(dhyara::delay_type), reinterpret_cast<std::uint8_t*>(&delay));
            input  += sizeof(dhyara::delay_type);
            length -= sizeof(dhyara::delay_type);
            packet.delay(delay);
        }
        if(length >= sizeof(std::uint8_t)){
            std::copy_n(input, sizeof(std::uint8_t), reinterpret_cast<std::uint8_t*>(&len));
            input  += sizeof(std::uint8_t);
            length -= sizeof(std::uint8_t);
        }
        if(length >= len){
            std::copy_n(input, len, std::back_inserter(name));
            input  += len;
            length -= len;
            packet.name(name);
        }
        return input;
    }
};

}

#endif // DHYARA_PACKETS_ADVERTISEMENT_H
