/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ROUTING_H
#define DHYARA_ROUTING_H

#include "dhyara/defs.h"
#include "dhyara/peer.h"
#include "dhyara/xqueue.h"
#include <map>
#include <limits>
#include <thread>
#include <mutex>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace dhyara{

/**
 * routing table
 * \ingroup routing
 */
struct routing {
    
    /**
     * metric of a route
     */
    struct route_metric{
        /**
         * construct a route metric with delay and last updated time
         */
        inline explicit route_metric(const dhyara::delay_type& delay, const dhyara::delay_type& updated): _delay(delay), _updated(updated){}
        /**
         * one trip delay encountered in this route
         */
        inline const dhyara::delay_type& delay() const { return _delay; }
        /**
         * last updated time
         */
        inline const dhyara::delay_type& updated() const { return _updated; }
        /**
         * update the last updated time
         */
        inline void update() { _updated = esp_timer_get_time(); }
        /**
         * update the route with a delay. Set sync_updated to true to secretly update the delay but not the last updated time. Doing so will stop the propagation of the change in route, although the delay has been updated.
         * \param delay new delay
         * \param sync_updated if set to false then do not update the last updated time
         */
        void update(dhyara::delay_type delay, bool sync_updated = true);
        /**
         * sortable by delay
         */
        inline bool operator<(const route_metric& other) const { return _delay < other._delay; }
        
        private:
            dhyara::delay_type _delay;
            dhyara::delay_type _updated;
    };
    
    /**
     * A route to a destination via a node (A null via should be considered as one hop)
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

    typedef std::map<route, route_metric> table_type;
    
    /**
     * The best next hop for the given target
     */
    struct next_hop{
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
         * construct using an intermediate node address and delay
         * \param via intermediate node address 
         * \param delay delay
         */
        inline next_hop(dhyara::address via, delay_type delay): _via(via), _delay(delay){}
        /**
         * Comparator
         */
        bool operator==(const next_hop& other) const;
        /**
         * Comparator
         */
        inline bool operator!=(const next_hop& other) const { return !(*this == other); }
        
        private:
            dhyara::address _via;
            delay_type _delay;
    };
    
    typedef std::map<dhyara::address, next_hop> next_vector_type;
    
    /**
     * Construct 
     */
    inline explicit routing(delay_type def = std::numeric_limits<delay_type>::max()): _def(def){}
    
    /**
     * checks whether there exists any route for the given destination
     * @param dst the final destination
     */
    bool exists(const dhyara::address& dst) const;
    /**
     * checks whether an rtt is associated with the given route
     * @param r the route (combination of destination and next hop)
     */
    bool exists(const route& r) const;
    /**
     * updates routing table with rtt of a route. 
     * returning true signifies that min values for the destination has been altered due to this route.
     * returnign false signifies that the min values are still the same
     */
    bool update(const route& r, const delay_type& d);
    /**
     * depritiate a route
     */
    bool depreciate(const route& r);
    /**
     * next node to for the final destination dst
     */
    dhyara::routing::next_hop next(const dhyara::address& dst) const;
    /**
     * depreciate a route that was not updated within dhyara::route_expiry time
     */
    void depreciate();
    /**
     * depreciate a route that was not updated within dhyara::route_expiry time
     */
    void depreciate(std::function<void (const dhyara::routing::route&, delay_type)> notify){
        dhyara::delay_type now = esp_timer_get_time();
        std::vector<std::pair<dhyara::routing::route, delay_type>> inactives;
        for(auto& kv: _table){
            auto route = kv.first;
            dhyara::delay_type delta = now - kv.second.updated();
            if(delta > dhyara::route_expiry){
                if(depreciate(route)){
                    inactives.push_back(std::make_pair(route, delay(route)));
                }
            }   
        }
        for(const auto& node: inactives){
            notify(node.first, node.second);
        }
    }
    
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
     * begin of the next best vector
     */
    inline next_vector_type::const_iterator next_begin() const { return _next.begin(); }
    /**
     * end of the next best vector
     */
    inline next_vector_type::const_iterator next_end() const { return _next.end(); }

    /**
     * Last time the routing table was updated
     */
    dhyara::delay_type lost_since() const;
    
    private:
        delay_type       _def;
        table_type       _table;
        next_vector_type _next;
        std::mutex       _mutex;
        
        delay_type delay(const route& r) const;
        next_hop calculated_next(dhyara::address dst) const;
        bool update_next(dhyara::address dst);
};

std::ostream& operator<<(std::ostream& os, const dhyara::routing::route& route);
std::ostream& operator<<(std::ostream& os, const dhyara::routing::next_hop& hop);
std::ostream& operator<<(std::ostream& os, const dhyara::routing& routing);

}

#endif // DHYARA_ROUTING_H
