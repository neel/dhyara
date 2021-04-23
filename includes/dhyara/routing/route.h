/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
