/*
 * Copyright (c) 2021, <copyright holder> <email>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  * Neither the name of the <organization> nor the
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
