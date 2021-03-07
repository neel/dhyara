/*
 * Copyright (c) 2021, <copyright holder> <email>
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

#ifndef DHYARA_NEIGHBOURHOOD_H
#define DHYARA_NEIGHBOURHOOD_H

#include "dhyara/peer.h"

namespace dhyara{

/**
 * Neighbourhood of a node
 */
struct neighbourhood{
    typedef std::set<peer> peer_collection_type;
    
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
    
    /**
     * return address of a node if it exists in neighbourhood or returns null address
     * \param addr string address 
     */
    dhyara::peer_address address(const std::string& addr) const;
    /**
     * return address of a node if it exists in neighbourhood or returns null address
     * \param addr string address 
     */
    dhyara::peer_address address(const dhyara::peer::address& addr) const;
    
    inline peer_collection_type::const_iterator begin() const { return _peers.begin(); }
    inline peer_collection_type::const_iterator end() const { return _peers.end(); }
    
    private:
        peer_collection_type   _peers;
    
};

}

#endif // DHYARA_NEIGHBOURHOOD_H
