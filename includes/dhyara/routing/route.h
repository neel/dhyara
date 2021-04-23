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

#ifndef DHYARA_ROUTING_ROUTE_H
#define DHYARA_ROUTING_ROUTE_H

#include "dhyara/defs.h"
#include "dhyara/address.h"

namespace dhyara{
    
namespace routing{
    
/**
 * A route to a destination via a node (A null via should be considered as one hop)
 * \ingroup routing
 */
struct route{
    /**
     * default construction prohibited. Construct only using destination and intermediate address pair.
     */
    route() = delete;
    /**
     * construct using a destination address and an intermediate node address. Set the intermediate node address to null (00:00:00:00:00:00) to indicate direct route to the destination.
     * \param dst destination address
     * \param via intermediate node address
     */
    route(const dhyara::address& dst, const dhyara::address& via);
        
    /**
     * destination address of this route
     */
    inline const dhyara::address& dst() const { return _dst; }
    /**
     * intermediate node address of this route
     */
    inline const dhyara::address& via() const { return _via; }
    
    /**
     * routes are sortable
     */
    bool operator<(const route& other) const;
    /**
     * comparable
     */
    bool operator==(const route& other) const;
    /**
     * comparable
     */
    inline bool operator!=(const route& other) const { return !(*this == other); }
    /**
     * convert to string
     */
    std::string to_string() const;
    
    private:
        dhyara::address _dst;
        dhyara::address _via;
};

std::ostream& operator<<(std::ostream& os, const dhyara::routing::route& route);
    
}

}

#endif // DHYARA_ROUTING_ROUTE_H
