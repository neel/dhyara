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
#include <dhyara/address.h>
#include "esp_now.h"

namespace dhyara{
    
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
        address             _addr;
        std::int8_t         _rssi;
        std::string         _name;
};

std::ostream& operator<<(std::ostream& os, const dhyara::peer& peer);
    
}

namespace std {

template <>
struct hash<dhyara::peer>{
    std::uint64_t operator()(const dhyara::peer& peer) const{
        return std::hash<std::uint64_t>()(peer.addr().hash());
    }
};

}

#endif // DHYARA_PEER_H
