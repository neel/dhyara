/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_NEIGHBOURHOOD_H
#define DHYARA_NEIGHBOURHOOD_H

#include "dhyara/peer.h"
#include "dhyara/neighbour.h"
#include <unordered_set>
#include <unordered_map>

namespace dhyara{

/**
 * Neighbourhood of a node
 * \ingroup dhyara 
 */
struct neighbourhood{
    typedef std::unordered_map<address, dhyara::neighbour> neighbour_collection_type;
    
    /**
     * add a peer as neighbour 
     * \param p peer
     */
    const dhyara::neighbour& add(const dhyara::neighbour& p);
    /**
     * conveniance overload to add a peer
     * \param addr peer address as string
     * \param ch WiFi channel
     * \param enc encryption
     */
    const dhyara::neighbour& add(const std::string& addr, std::uint8_t ch, bool enc = false);
    /**
     * conveniance overload to add a peer
     * \param addr peer address 
     * \param ch WiFi channel
     * \param enc encryption
     */
    const dhyara::neighbour& add(const dhyara::address& addr, uint8_t ch, bool enc = false);
    /**
     * check whether a peer is in neighbourhood
     * \param addr peer address
     */
    bool exists(const std::string& addr) const;
    /**
     * check whether a peer is in neighbourhood
     * \param addr peer address
     */
    bool exists(const dhyara::address& addr) const;
    
    /**
     * number of immediate neighbours
     */
    std::size_t size() const;
    /**
     * returns a peer by its address (assuming that it exists in the neighbourhood)
     * \note use exists() to check whether the address exists before calling this function
     */
    dhyara::neighbour& neighbour(const dhyara::address& addr);
    
    /**
     * begin iterator
     */
    inline neighbour_collection_type::const_iterator begin() const { return _neighbours.begin(); }
    /**
     * end iterator
     */
    inline neighbour_collection_type::const_iterator end() const { return _neighbours.end(); }
    
    private:
        neighbour_collection_type   _neighbours;
    
};

}

#endif // DHYARA_NEIGHBOURHOOD_H
