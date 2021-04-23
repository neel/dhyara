/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ROUTING_NEXT_HOP_H
#define DHYARA_ROUTING_NEXT_HOP_H

#include "dhyara/defs.h"
#include "dhyara/address.h"

namespace dhyara{
    
namespace routing{
    
/**
 * The best next hop for the given target
 * \ingroup routing
 */
struct next_hop{       
    /**
     * construct using an intermediate node address and delay
     * \param via intermediate node address 
     * \param delay delay
     */
    inline next_hop(dhyara::address via, delay_type delay): _via(via), _delay(delay), _published(delay){}
    
    /**
     * Construct from a pair
     */
    inline next_hop(const std::pair<dhyara::address, dhyara::delay_type>& pair): next_hop(pair.first, pair.second){}
    
    /**
     * assign by a pair
     */
    inline next_hop& operator=(const std::pair<dhyara::address, dhyara::delay_type>& pair){
        _via = pair.first;
        _delay = pair.second;
        return *this;
    }
    
    /**
     * returns the best next hop address for the given target
     * \return dhyara::address
     */
    inline const dhyara::address& via() const { return _via; }
    /**
     * returns the best next hop delay for the given target
     */
    inline const delay_type& delay() const { return _delay; }
    /**
     * returns the last published delay for the given target
     */
    inline const delay_type& published() const { return _published; }
    /**
     * Set the current delay as published
     */
    inline void publish() { _published = _delay; }
    /**
     * Comparator
     */
    inline bool operator==(const next_hop& other) const { return _via == other._via && _delay == other._delay; }
    /**
     * Comparator
     */
    inline bool operator!=(const next_hop& other) const { return !(*this == other); }
    
    private:
        dhyara::address _via;
        delay_type      _delay;
        delay_type      _published;
};
    
std::ostream& operator<<(std::ostream& os, const dhyara::routing::next_hop& hop);

}
    
}

#endif // DHYARA_ROUTING_NEXT_HOP_H
