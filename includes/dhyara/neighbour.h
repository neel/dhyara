/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_NEIGHBOUR_H
#define DHYARA_NEIGHBOUR_H

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
#include <dhyara/peer.h>
#include "esp_now.h"

namespace dhyara{
    
/**
 * An immediate neighbour of the node
 * \code
 * dhyara::neighbour neighbour("ff:ff:ff:ff:ff:ff");
 * neighbour.encrypt(false).channel(1);
 * neighbour.addr().to_string(); // returns "ff:ff:ff:ff:ff:ff"
 * neighbour.encrypt(); // returns false
 * neighbour.channel(); // returns 1
 * \endcode
 * \ingroup dhyara 
 */
struct neighbour: dhyara::peer{
    /**
     * Construct a neighbour
     * \param addr address
     * \param ch WiFi channel
     * \param enc encryption
     */
    neighbour(const dhyara::address& addr, std::uint8_t ch=1, bool enc = false);
    /**
     * Construct a neighbour
     * \param addr address
     * \param ch WiFi channel
     * \param enc encryption
     */
    neighbour(const std::string& addr, std::uint8_t ch=1, bool enc = false);
    
    /**
     * set encryption flag
     */
    inline neighbour& encrypt(bool flag){_peer.encrypt = flag; return *this;}
    /**
     * get encryption flag
     */
    inline bool encrypt() const {return _peer.encrypt;}
    
    /**
     * set peer channel
     */
    inline neighbour& channel(std::uint8_t c){_peer.channel = c; return *this;}
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

    inline const esp_now_peer_info_t* raw() const {return &_peer;}
    
    private:
        esp_now_peer_info_t _peer;
        std::int8_t         _rssi;
};

std::ostream& operator<<(std::ostream& os, const dhyara::neighbour& peer);
    
}
namespace std {

template <>
struct hash<dhyara::neighbour>{
    std::uint64_t operator()(const dhyara::neighbour& neighbour) const{
        return std::hash<std::uint64_t>()(neighbour.addr().hash());
    }
};

}

#endif // DHYARA_NEIGHBOUR_H
