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
#include <unordered_set>
#include <unordered_map>

namespace dhyara{

/**
 * Neighbourhood of a node
 */
struct neighbourhood{
    typedef std::unordered_map<peer_address, peer> peer_collection_type;
    
    /**
     * add a peer as neighbour 
     * \param p peer
     */
    const dhyara::peer& add(const peer& p);
    /**
     * conveniance overload to add a peer
     * \param addr peer address as string
     * \param ch WiFi channel
     * \param enc encryption
     */
    const dhyara::peer& add(const std::string& addr, std::uint8_t ch, bool enc = false);
    /**
     * conveniance overload to add a peer
     * \param addr peer address 
     * \param ch WiFi channel
     * \param enc encryption
     */
    const dhyara::peer& add(const dhyara::peer::address& addr, uint8_t ch, bool enc = false);
    /**
     * check whether a peer is in neighbourhood
     * \param addr peer address
     */
    bool exists(const std::string& addr) const;
    /**
     * check whether a peer is in neighbourhood
     * \param addr peer address
     */
    bool exists(const dhyara::peer::address& addr) const;
    
    /**
     * number of immediate neighbours
     */
    std::size_t size() const;
    
//     /**
//      * return address of a node if it exists in neighbourhood or returns null address
//      * \param addr string address 
//      */
//     dhyara::peer_address address(const std::string& addr) const;
//     /**
//      * return address of a node if it exists in neighbourhood or returns null address
//      * \param addr string address 
//      */
//     dhyara::peer_address address(const dhyara::peer::address& addr) const;
    /**
     * 
     */
    dhyara::peer& get_peer(const dhyara::peer::address& addr);
    
    /**
     * begin iterator
     */
    inline peer_collection_type::const_iterator begin() const { return _peers.begin(); }
    /**
     * end iterator
     */
    inline peer_collection_type::const_iterator end() const { return _peers.end(); }
    
    private:
        peer_collection_type   _peers;
    
};

}

#endif // DHYARA_NEIGHBOURHOOD_H
