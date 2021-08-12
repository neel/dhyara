/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SYNCHRONIZER_H
#define DHYARA_SYNCHRONIZER_H

#include <map>
#include "dhyara/peer.h"
#include "dhyara/defs.h"
#include "dhyara/xqueue.h"

namespace dhyara{
    
struct link;

/**
 * A synchronization candidate is defined by a route (self -> via -> destination) and an integer denoting one trip delay. The via is set to null to denote direct connectivity between self and destination.
 * \ingroup routing
 */
struct candidate{
    dhyara::address       _dest;
    dhyara::address       _via;
    dhyara::delay_type    _delay;
    std::uint8_t          _hops;
    
    inline candidate(): _dest(dhyara::address::null()), _via(dhyara::address::null()), _delay(0), _hops(0) {}
    inline candidate(dhyara::address dest, dhyara::address via, dhyara::delay_type delay, std::uint8_t hops): _dest(dest), _via(via), _delay(delay), _hops(hops){}
    
    /**
     * destination address
     */
    inline dhyara::address dest() const { return _dest; }
    /**
     * intermediate node address
     */
    inline dhyara::address via() const { return _via; }
    /**
     * one trip delay
     */
    inline dhyara::delay_type delay() const { return _delay; }
    /**
     * number of hops encountered between via and dest
     */
    inline std::uint8_t hops() const { return _hops; }
};

/**
 * Synchronize routes accross all nodes in the network. 
 * A synchronization candidate is enqueued.  
 * Synchronization candidate is defined by a route (self -> via -> destination) and an integer denoting one trip delay. 
 * The via is set to null to denote direct connectivity between self and destination.
 * The run function (never returns) should be called from a task to dequeue the enqueued candidates and sync accross all nodes. 
 * Once dequeued, the routing table is updated using the candidate's delay. 
 * 
 * The delay associated with the existing route (dest, via) is updated to the new delay (from the dequeued candidate) through the routing table (The route will be created if it does not exist already).
 * The update operation in the routing table returns a boolean value depicting whether synchronization is required or not (best next hop for the destination is altered).
 * If synchronization is required then finds the current next (best immediate hop) for the dest (which has been changed due to previous update).
 * Constructs an advertisement packet with the delay associated with the current next.
 * Sends that advertisement packet to all neighbours except the dest node.
 * 
 * Additionally it also maintains a map to maintain last advertisement time. 
 * If the update on the routing table responds true, denoting synchronization is required then the last advertisement time assocated with the dest is also updated.
 * If the update returns false then this last advertisement time assocated with the dest is checked.
 * If it is found out that no advertisement has been sent for this dest for too long (advertisement_expiry) then another advertisement is sent.
 * 
 * \ingroup routing
 * \note Not Default Constructible, Not Copyable, Not Copy Assignable.
 * \see dhyara::candidate
 */
struct synchronizer{
    typedef dhyara::xqueue<candidate, dhyara::sync_queue_size> sync_queue_type;
    typedef std::map<dhyara::address, dhyara::delay_type> last_advertisement_map;
    
    /**
     * Construct using a link
     */
    synchronizer(dhyara::link& link);
    synchronizer(const synchronizer&) = delete;
    synchronizer& operator=(const synchronizer&) = delete;
    
    /**
     * enqueue a candidate 
     * 
     * \param dest destination address
     * \param via intermediate address
     * \param delay one trip delay in that route 
     * \param hops number of hops
     */
    void queue(const dhyara::address& dest, const dhyara::address& via, dhyara::delay_type delay, std::uint8_t hops);
    /**
     * enqueue a candidate 
     * 
     * \param c candidate
     */
    void queue(const dhyara::candidate& c);
    /**
     * Dequeue the candidates and sync. Never returns. 
     * \attention Should be called from a FreeRTOS Task.
     * Calls \ref sync on each dequeued candidate.
     */
    void run();
    
    private:
        /**
         * Updates delay associated with the existing route (dest, via) to delay through the routing table (The route will be created if it does not exist already).
         * The update operation in the routing table returns a boolean value depicting whether synchronization is required or not.
         * If synchronization is required then finds the current next (best immediate hop) for the dest (which has been changed due to previous update).
         * Constructs an advertisement packet with the delay associated with the current next.
         * Sends that advertisement packet to all neighbours except the dest node.
         * 
         * Additionally it also maintains a map to maintain last advertisement time. 
         * If the update on the routing table responds true, denoting synchronization is required then the last advertisement time assocated with the dest is also updated.
         * If the update returns false then this last advertisement time assocated with the dest is checked.
         * If it is found out that no advertisement has been sent for this dest for too long (advertisement_expiry) then another advertisement is sent.
         *
         * \param dest The destination address
         * \param via The intermediate node address
         * \param delay the one trip delay on this route 
         * \param hops hops
         */
        void sync(const dhyara::address& dest, const dhyara::address& via, dhyara::delay_type delay, std::uint8_t hops);
    private:
        dhyara::link&           _link;
        sync_queue_type         _fifo;
        last_advertisement_map  _last;
};

}

#endif // DHYARA_SYNCHRONIZER_H
