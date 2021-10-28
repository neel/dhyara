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

#ifdef CONFIG_DISABLE_SEND_QUEUEING
    #define DHYARA_DISABLED_SEND_QUEUEING 1
#else
    #define DHYARA_DISABLED_SEND_QUEUEING 0
#endif 
#define DHYARA_ENABLED_SEND_QUEUEING !DHYARA_DISABLED_SEND_QUEUEING

#ifdef CONFIG_ENABLE_HTTP_MANAGEMENT_SERVER
    #define DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER 1
#else
    #define DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER 0
#endif 
#define DHYARA_DISABLED_HTTP_MANAGEMENT_SERVER !DHYARA_ENABLED_HTTP_MANAGEMENT_SERVER

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
     * while depretiating a route the current delay will be multiplied with this coefficient
     */
    constexpr const std::uint8_t depreciation_coefficient = CONFIG_DHYARA_DEPRECIATION_COEFFICIENT;
    /**
     * Advertisement expiry
     */
    constexpr const delay_type advertisement_expiry = CONFIG_DHYARA_ADVERTISEMENT_EXPIRY * 1000; // in us
    /**
     * Route expiry
     */
    constexpr const delay_type route_expiry = advertisement_expiry + (CONFIG_ROUTE_EXPIRY_COEFFICIENT * beacon_interval); // us
    
    /**
     * Different policies to apply on inactive routes
     */
    enum class depreciation_policies{
        /**
         * double the delay if a route is inactive for \ref route_expiry period of time
         */
        amplify_delay,
        /**
         * remove a route that is inactive for \ref route_expiry period of time
         */
        remove_route
    };
    
    /**
     * policy to apply on inactive routes
     */
    constexpr const depreciation_policies depreciation_policy = depreciation_policies::remove_route;
    
    constexpr const std::size_t udp_relay_rcv_buffer_size = CONFIG_DHYARA_UTIL_UDP_RELAY_BUFFER_SIZE;
}

#endif // DHYARA_DEFS_H
