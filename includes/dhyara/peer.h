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

#ifndef DHYARA_PEER_H
#define DHYARA_PEER_H

#include <set>
#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <functional>
#include "esp_now.h"

namespace dhyara{
    
/**
 * peer address (mac address)
 */
struct peer_address{
    typedef std::array<std::uint8_t, 6> container_type;
    
    /**
     * construct mac address by an array
     */
    explicit peer_address(const container_type& addr);
    /**
     * construct mac address by octets
     */
    peer_address(std::uint8_t b1, std::uint8_t b2, std::uint8_t b3, std::uint8_t b4, std::uint8_t b5, std::uint8_t b6);
    /**
     * construct mac address by providing address to an array
     */
    explicit peer_address(const std::uint8_t* bytes);
    /**
     * construct mac address by string
     */
    explicit peer_address(const std::string& addr);
    
    /**
     * null mac
     */
    inline static peer_address null() { return peer_address(0x0, 0x0, 0x0, 0x0, 0x0, 0x0); }
    /**
     * broadcast mac
     */
    inline static peer_address all() { return peer_address(0xff, 0xff, 0xff, 0xff, 0xff, 0xff); }
    
    /**
     * convert to string
     */
    std::string to_string() const;
    /**
     * raw array
     */
    inline const std::uint8_t* raw() const {return _bytes.data();}
    /**
     * first octet of the mac address
     */
    inline std::uint8_t b1() const {return _bytes[0];}
    /**
     * second octet of the mac address
     */
    inline std::uint8_t b2() const {return _bytes[1];}
    /**
     * third octet of the mac address
     */
    inline std::uint8_t b3() const {return _bytes[2];}
    /**
     * fourth octet of the mac address
     */
    inline std::uint8_t b4() const {return _bytes[3];}
    /**
     * fifth octet of the mac address
     */
    inline std::uint8_t b5() const {return _bytes[4];}
    /**
     * sixth octed  of the mac address
     */
    inline std::uint8_t b6() const {return _bytes[5];}
    /**
     * 64 bit hash value for the mac address
     */
    std::uint64_t hash() const;
    bool operator<(const peer_address& other) const;
    bool operator==(const peer_address& other) const;
    bool operator==(const std::string& addr) const;
    inline bool operator!=(const peer_address& other) const { return !(*this == other); }
    inline bool operator!=(const std::string& addr) const { return !(*this == addr); }
    
    void set(const std::uint8_t* bytes);
    
    /**
     * check whether the mac address is null 
     */
    inline bool is_null() const { return std::accumulate(_bytes.begin(), _bytes.end(), 0) == 0; }
    /**
     * check whether the mac address is broadcast
     */
    inline bool is_broadcast() const { return std::all_of(_bytes.begin(), _bytes.end(), std::bind(std::equal_to<std::uint8_t>(), std::placeholders::_1, 0xff)); }
    
    private:
        container_type _bytes;
};

/**
 * A peer communicating in the medium
 * @code
 * dhyara::peer p ("ff:ff:ff:ff:ff:ff");
 * p.encrypt(false).channel(1);
 * 
 * p.addr().to_string(); // returns "ff:ff:ff:ff:ff:ff"
 * p.encrypt(); // returns false
 * p.channel(); // returns 1
 * @endcode
 */
struct peer{
    typedef peer self_type;
    typedef peer_address address;
        
    explicit peer(const address& addr);
    peer(const std::string& addr, std::uint8_t ch, bool enc = false);
    
    /**
     * get peer address
     */
    inline const address& addr() const {return _addr;}
    
    /**
     * set encryption flag
     */
    inline self_type& encrypt(bool flag){_peer.encrypt = flag; return *this;}
    /**
     * get encryption flag
     */
    inline bool encrypt() const {return _peer.encrypt;}
    
    /**
     * set peer channel
     */
    inline self_type& channel(std::uint8_t c){_peer.channel = c; return *this;}
    /**
     * get peer channel
     */
    inline bool channel() const {return _peer.channel;}
    
    inline const esp_now_peer_info_t* raw() const {return &_peer;}
    
    bool operator<(const peer& other) const;
    
    private:
        esp_now_peer_info_t _peer;
        address             _addr;
};

std::ostream& operator<<(std::ostream& os, const dhyara::peer::address& address);
std::ostream& operator<<(std::ostream& os, const dhyara::peer& peer);
    
}

#endif // DHYARA_PEER_H
