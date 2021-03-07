#ifndef DEFS_H
#define DEFS_H

#include <cstdint>

namespace dhyara {
    typedef std::uint64_t delay_type;
    /**
     * Capacity of send/receive queue
     */
    constexpr const std::uint64_t queue_size = 256;
    /**
     * Capacity of synchronization queue 
     */
    constexpr const std::uint64_t sync_queue_size = 64;
    /**
     * WiFi channel for broadcast
     */
    constexpr const std::uint8_t espnow_broadcast_channel = 1;
    /**
     * peer channel
     */
    constexpr const std::uint8_t espnow_peer_channel = 1;
    /**
     * beacon interval
     */
    constexpr const delay_type beacon_interval = 2000; // in ms
    /**
     * The amount of change in delay, that is ignored and assumed as no change in route
     */
    constexpr const delay_type delay_tolerance = 0;
    /**
     * Advertisement expiry
     */
    constexpr const delay_type advertisement_expiry = 2000*1000; // in us
    /**
     * Route expiry
     */
    constexpr const delay_type route_expiry = advertisement_expiry + (1000*beacon_interval); // us
}

#endif // DEFS_H
