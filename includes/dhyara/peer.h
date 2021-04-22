/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
 * \ingroup dhyara
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
    /**
     * Less than comparator
     */
    bool operator<(const peer_address& other) const;
    /**
     * Comparator
     */
    bool operator==(const peer_address& other) const;
    /**
     * Compare with string
     */
    bool operator==(const std::string& addr) const;
    /**
     * Comparator
     */
    inline bool operator!=(const peer_address& other) const { return !(*this == other); }
    /**
     * Comparator
     */
    inline bool operator!=(const std::string& addr) const { return !(*this == addr); }
    
    /**
     * Set the octets directly
     */
    void set(const std::uint8_t* bytes);
    
    /**
     * Copy the MAC address to another container by bytes
     */
    template <typename OutIt>
    OutIt copy(OutIt it) const{
        return std::copy(_bytes.begin(), _bytes.end(), it);
    }
    
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
        
    explicit peer(const peer_address& addr);
    peer(const std::string& addr, std::uint8_t ch, bool enc = false);
    
    /**
     * get peer address
     */
    inline const peer_address& addr() const {return _addr;}
    
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
    inline std::uint8_t channel() const {return _peer.channel;}
    /**
     * Get RSSI value for the peer
     */
    inline std::int8_t rssi() const { return _rssi; }
    /**
     * Set RSSI value for the peer
     */
    inline void rssi(std::uint8_t v) { _rssi = v; }
    
    inline void name(const std::string& n) { _name = n; }
    
    inline const std::string& name() const { return _name; }
    
    inline const esp_now_peer_info_t* raw() const {return &_peer;}
    
    bool operator<(const peer& other) const;
    /**
     * Comparator
     */
    bool operator==(const peer& other) const;
    /**
     * Comparator
     */
    inline bool operator!=(const peer& other) const { return !(*this == other); }
    
    private:
        esp_now_peer_info_t _peer;
        peer_address        _addr;
        std::int8_t         _rssi;
        std::string         _name;
};

std::ostream& operator<<(std::ostream& os, const dhyara::peer_address& address);
std::ostream& operator<<(std::ostream& os, const dhyara::peer& peer);
    
}

namespace std {

template <>
struct hash<dhyara::peer_address>{
    std::uint64_t operator()(const dhyara::peer_address& addr) const{
        return std::hash<std::uint64_t>()(addr.hash());
    }
};

template <>
struct hash<dhyara::peer>{
    std::uint64_t operator()(const dhyara::peer& peer) const{
        return std::hash<std::uint64_t>()(peer.addr().hash());
    }
};

}

#endif // DHYARA_PEER_H
