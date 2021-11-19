/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ROUTING_TABLE_H
#define DHYARA_ROUTING_TABLE_H

#include <map>
#include <limits>
#include <mutex>
#include <functional>
#include "dhyara/defs.h"
#include "dhyara/address.h"
#include "dhyara/routing/metric.h"
#include "dhyara/routing/next_hop.h"
#include "dhyara/routing/route.h"

namespace dhyara{
    
struct synchronizer;
    
namespace routing{
    
/**
 * routing table
 * \ingroup routing
 */
struct table {
    typedef std::map<dhyara::routing::route, dhyara::routing::metric> table_type;
    typedef std::map<dhyara::address, dhyara::routing::next_hop> next_vector_type;
    
    /**
     * Construct 
     */
    inline explicit table(dhyara::delay_type def = std::numeric_limits<dhyara::delay_type>::max()): _def(def){}
    
    /**
     * checks whether there exists any route for the given destination
     * @param dst the final destination
     */
    bool exists(const dhyara::address& dst) const;
    /**
     * checks whether an rtt is associated with the given route
     * @param r the route (combination of destination and next hop)
     */
    bool exists(const dhyara::routing::route& r) const;
    /**
     * @brief Whether there exists any route to dst via the given intermediate node
     * 
     * @param dst 
     * @param via 
     * @return true 
     * @return false 
     */
    bool reachable(const dhyara::address& dst, const dhyara::address& via = dhyara::address::null()) const;
    /**
     * Best next node to for the final destination dst
     * \note If the node is not found in the best intermediate table then returns an invalid next_hop `(dst, _def, 0)` where dst itself is used as the intermediate node, _def is the default delay, and 0 is the number of hops
     * \param dst destinatin node
     */
    dhyara::routing::next_hop next(const dhyara::address& dst) const;
    /**
     * updates routing table with rtt of a route. 
     * returning true signifies that min values for the destination has been altered due to this route.
     * returnign false signifies that the min values are still the same
     */
    bool update(const dhyara::routing::route& r, const delay_type& d, std::uint8_t hops);
    /**
     * Removes routes that has not been updated within dhyara::route_expiry time.
     * \warning This depreciate function removes an existing route. Use \ref depreciate(std::function<void (const dhyara::routing::route&, dhyara::delay_type, std::uint8_t)> notify) if you don't want to remove.
     */
    void depreciate(dhyara::synchronizer& synchronizer);
    
    /**
     * print the routing table and current next hop vector
     */
    std::ostream& print(std::ostream& os) const;
    
    /**
     * begin of the routing table
     */
    inline table_type::const_iterator route_begin() const { return _table.begin(); }
    /**
     * end of the routing table
     */
    inline table_type::const_iterator route_end() const { return _table.end(); }
    /**
     * number of routes in the routing table
     */
    inline table_type::size_type routes_size() const { return _table.size(); }
    /**
     * begin of the next best vector
     */
    inline next_vector_type::const_iterator next_begin() const { return _next.begin(); }
    /**
     * end of the next best vector
     */
    inline next_vector_type::const_iterator next_end() const { return _next.end(); }
    /**
     * number of best routes in the routing table
     */
    inline table_type::size_type next_size() const { return _next.size(); }
    /**
     * Last time the routing table was updated
     */
    dhyara::delay_type lost_since() const;
    
    private:
        delay_type         _def;
        table_type         _table;
        next_vector_type   _next;
        mutable std::mutex _mutex;
        
        /**
         * Finds the given route in the routing table and returns its delay. If there is no such route then returns the default delay.
         * \param r the route
         */
        dhyara::delay_type delay(const dhyara::routing::route& r) const;
        /**
         * Finds the given route in the routing table and returns its delay. If there is no such route then returns the default delay.
         * \param r the route
         */
        std::uint8_t hops(const dhyara::routing::route& r) const;
        /**
         * Calculates the best next hop from all existing routes in the routing table, that lead to the given destination.
         * \note The function only returns a calculated next hop. But it does not alter the previously calculated next hop.
         * \attention If there is no route in the routing table, that lead to the given destination, then returns a next hop with default delay.
         * \param dst The destination node address
         * \warning the function is NOT gaurded by mutex.
         */
        dhyara::routing::next_hop calculated_next(dhyara::address dst) const;
        /**
         * Updates the next hop of the given destination using the best route from the existing routing table.
         * returns true if either the best intermediate node has changed or the change in delay is beyond tolerated delay.
         * \param dst The destination node address
         * \see calculated_next
         * \warning the function is NOT gaurded by mutex.
         */
        bool update_next(dhyara::address dst);
};
    
std::ostream& operator<<(std::ostream& os, const dhyara::routing::table& routing);

}
    
}

#endif // DHYARA_ROUTING_TABLE_H
