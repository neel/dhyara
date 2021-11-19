/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_UNIVERSE_H
#define DHYARA_UNIVERSE_H

#include "dhyara/peer.h"
#include <unordered_set>
#include <unordered_map>

namespace dhyara{

namespace routing{
    struct table;
}

/**
 * Universe of all nodes in the known in the universe
 * \ingroup dhyara 
 */
struct universe{
    typedef std::unordered_map<address, dhyara::peer> peer_collection_type;
    
    /**
     * add a peer as neighbour 
     * \param p peer
     */
    const dhyara::peer& add(const dhyara::peer& p);
    /**
     * conveniance overload to add a peer
     * \param addr peer address as string
     */
    const dhyara::peer& add(const std::string& addr);
    /**
     * conveniance overload to add a peer
     * \param addr peer address
     */
    const dhyara::peer& add(const dhyara::address& addr);
    /**
     * remove a node from universe
     * \param addr peer address
     */
    bool remove(const dhyara::address& addr);
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
     * \attention use exists() to check whether the address exists before calling this function
     */
    dhyara::peer& peer(const dhyara::address& addr);
    
    /**
     * begin iterator
     */
    inline peer_collection_type::const_iterator begin() const { return _peers.begin(); }
    /**
     * end iterator
     */
    inline peer_collection_type::const_iterator end() const { return _peers.end(); }
    /**
     * @brief remove the nodes from universe that are not present in the routing table
     * 
     * @param table 
     * @return std::size_t 
     */
    std::size_t remove_unreachables(const dhyara::routing::table& routes);
    private:
        peer_collection_type   _peers;
    
};

}

#endif // DHYARA_UNIVERSE_H
