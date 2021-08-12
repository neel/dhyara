/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ROUTING_METRIC_H
#define DHYARA_ROUTING_METRIC_H

#include "dhyara/defs.h"
#include "esp_timer.h"

namespace dhyara{
    
namespace routing{
    
/**
 * metric of a route
 * \ingroup routing
 */
struct metric{
    /**
     * construct a route metric with delay and last updated time
     */
    inline explicit metric(const dhyara::delay_type& delay, std::uint8_t hops, const dhyara::delay_type& updated): _delay(delay), _hops(hops), _updated(updated){}
    /**
     * one trip delay encountered in this route
     */
    inline const dhyara::delay_type& delay() const { return _delay; }
    /**
     * one trip hops encountered in this route
     */
    inline std::uint8_t hops() const { return _hops; }
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
    void update(dhyara::delay_type delay, std::uint8_t hops = 0, bool sync_updated = true);
    /**
     * sortable by delay
     */
    inline bool operator<(const metric& other) const { return _hops < other.hops() || (_hops == other.hops() && _delay <= other._delay); }
    
    private:
        dhyara::delay_type _delay;
        std::uint8_t       _hops;
        dhyara::delay_type _updated;
};
    
}

}
#endif // DHYARA_ROUTING_METRIC_H
