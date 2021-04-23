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

#ifndef DHYARA_ROUTING_ROUTE_METRIC_H
#define DHYARA_ROUTING_ROUTE_METRIC_H

#include "dhyara/defs.h"
#include "esp_timer.h"

namespace dhyara{
    
namespace routing{
    
/**
 * metric of a route
 * \ingroup routing
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
    
}

}
#endif // DHYARA_ROUTING_ROUTE_METRIC_H
