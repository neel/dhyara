/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_DEFS_H
#define DHYARA_DEFS_H

#include "sdkconfig.h"
#include <cstdint>

namespace dhyara {
    typedef std::uint64_t delay_type;
    /**
     * Capacity of send/receive queue
     */
    constexpr const std::uint64_t queue_size = CONFIG_DHYARA_RECEIVE_QUEUE_SIZE;
    /**
     * Capacity of synchronization queue 
     */
    constexpr const std::uint64_t sync_queue_size = CONFIG_DHYARA_SYNC_QUEUE_SIZE;
    /**
     * WiFi channel for broadcast
     */
    constexpr const std::uint8_t espnow_broadcast_channel = CONFIG_DHYARA_BROADCAST_CHANNEL;
    /**
     * peer channel
     */
    constexpr const std::uint8_t espnow_peer_channel = CONFIG_DHYARA_PEER_CHANNEL;
    /**
     * beacon interval
     */
    constexpr const delay_type beacon_interval = CONFIG_DHYARA_BEACON_INTERVAL; // in ms
    /**
     * The amount of change in delay, that is ignored and assumed as no change in route
     */
    constexpr const delay_type delay_tolerance = CONFIG_DHYARA_DELAY_TOLERANCE;
    /**
     * Advertisement expiry
     */
    constexpr const delay_type advertisement_expiry = CONFIG_DHYARA_ADVERTISEMENT_EXPIRY * 1000; // in us
    /**
     * Route expiry
     */
    constexpr const delay_type route_expiry = advertisement_expiry + (CONFIG_ROUTE_EXPIRY_COEFFICIENT * beacon_interval); // us
}

#endif // DHYARA_DEFS_H
