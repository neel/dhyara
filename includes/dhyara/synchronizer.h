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
 */
struct candidate{
    dhyara::peer::address _dest;
    dhyara::peer::address _via;
    dhyara::delay_type    _delay;
    
    inline candidate(): _dest(dhyara::peer::address::null()), _via(dhyara::peer::address::null()), _delay(0) {}
    inline candidate(dhyara::peer::address dest, dhyara::peer::address via, dhyara::delay_type delay): _dest(dest), _via(via), _delay(delay){}
    
    /**
        * destination address
        */
    inline dhyara::peer::address dest() const { return _dest; }
    /**
        * intermediate node address
        */
    inline dhyara::peer::address via() const { return _via; }
    /**
        * one trip delay
        */
    inline dhyara::delay_type delay() const { return _delay; }
};

/**
 * Synchronize routes accross all nodes in the network. Non copyable. A synchronization candidate is enqueued. The run function (never returns) should be called from a task to dequeue the enqueued candidates and sync accross all nodes. A synchronization candidate is defined by a route (self -> via -> destination) and an integer denoting one trip delay. The via is set to null to denote direct connectivity between self and destination.
 * 
 * \see dhyara::candidate
 */
struct synchronizer{
    typedef dhyara::xqueue<candidate, dhyara::sync_queue_size> sync_queue_type;
    typedef std::map<dhyara::peer::address, dhyara::delay_type> last_advertisement_map;
    
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
     */
    void queue(const dhyara::peer::address& dest, const dhyara::peer::address& via, dhyara::delay_type delay);
    /**
     * enqueue a candidate 
     * 
     * \param c candidate
     */
    void queue(const dhyara::candidate& c);
    /**
     * Dequeue the candidates and sync. Never returns. Should be called from a FreeRTOS Task.
     */
    void run();
    
    private:
        void sync(const dhyara::peer::address& dest, const dhyara::peer::address& via, dhyara::delay_type delay);
    private:
        dhyara::link&           _link;
        sync_queue_type         _fifo;
        last_advertisement_map  _last;
};

}

#endif // DHYARA_SYNCHRONIZER_H
