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
#include "esp_timer.h"
#include <dhyara/defs.h>

namespace dhyara{
    
/**
 * A peer is identified by a mac address
 * \ingroup dhyara 
 */
struct peer{
    /**
     * @brief Construct a peer 
     * \param addr address
     */
    explicit peer(const address& addr);
    /**
     * @brief get peer address
     */
    inline const address& addr() const {return _addr;}
    /**
     * @brief Set Name
     */
    inline void name(const std::string& n) { _name = n; }
    /**
     * @brief Get Name
     */
    inline const std::string& name() const { return _name; }
    /**
     * @brief Less than comparator
     */
    bool operator<(const peer& other) const;
    /**
     * @brief Comparator
     */
    bool operator==(const peer& other) const;
    /**
     * @brief Comparator
     */
    inline bool operator!=(const peer& other) const { return !(*this == other); }
    
    private:
        address             _addr;
        std::string         _name;
};
    
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
