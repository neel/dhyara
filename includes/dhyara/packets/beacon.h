/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_BEACON_H
#define DHYARA_PACKETS_BEACON_H

#include <cstdint>
#include <algorithm>
#include "esp_timer.h"
#include "dhyara/packets/serialization.h"

namespace dhyara{
    
namespace packets{
    
/**
 * beacon packet
 * \ingroup packets
 */
struct beacon{
    std::uint64_t _time;
    std::string   _name;
    
    /**
     * Construct a beacon packet with the current time
     */
    inline beacon(): _time(esp_timer_get_time()){}
    /**
     * Construct a beacon packet with the current time and the name provided
     */
    inline beacon(const std::string& n): _time(esp_timer_get_time()), _name(n){}
    /**
     * Construct a beacon packet with the time provided
     */
    inline explicit beacon(std::uint64_t time): _time(time){}
    /**
     * Size of the beacon packet
     */
    inline std::size_t size() const {return sizeof(std::uint64_t) + sizeof(std::uint8_t) + _name.size();}
    /**
     * Time in the beacon packet
     */
    inline const std::uint64_t& time() const {return _time;}
    inline const std::string& name() const{ return _name; }
    inline void name(const std::string& n) { _name = n; }
};

}

template <>
struct serialization<packets::beacon>{
    template <typename OutIt>
    static OutIt write(const packets::beacon& packet, OutIt output){
        std::uint8_t len = packet._name.size();
        const std::uint8_t* ptime = reinterpret_cast<const std::uint8_t*>(&packet._time);
        const std::uint8_t* plen  = reinterpret_cast<const std::uint8_t*>(&len);
        output = std::copy_n(ptime, sizeof(packet._time), output);
        output = std::copy_n(plen, sizeof(len), output);
        output = std::copy_n(packet._name.begin(), len, output);
        return output;
    }
    template <typename InIt>
    static InIt read(packets::beacon& packet, InIt input, std::size_t length){
        std::uint8_t len = 0;
        if(length >= sizeof(packet._time)){
            std::copy_n(input, sizeof(packet._time), reinterpret_cast<std::uint8_t*>(&packet._time));
            input  += sizeof(packet._time);
            length -= sizeof(packet._time);
        }
        if(length >= sizeof(len)){
            std::copy_n(input, sizeof(len), reinterpret_cast<std::uint8_t*>(&len));
            input  += sizeof(len);
            length -= sizeof(len);
        }
        if(length >= len){
            std::copy_n(input, len, std::back_inserter(packet._name));
            input  += len;
            length -= len;
        }
        return input;
    }
};

}

#endif // DHYARA_PACKETS_BEACON_H
