/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_LINK_H
#define DHYARA_LINK_H

#include <map>
#include <functional>
#include "dhyara/peer.h"
#include "dhyara/frame.h"
#include "dhyara/routing.h"
#include "dhyara/packets.h"
#include "dhyara/neighbourhood.h"
#include "dhyara/xqueue.h"
#include "dhyara/utils/printer.h"

namespace dhyara{
   
/**
 * Link Layer Medium is responsible for sending packets and processing the received packets. All received data should be forwarded to the link via \ref _esp_sent_cb which wil then be processed through the installed actions.
 * \code
 * dhyara::link medium; // A communication medium 
 * 
 * dhyara::actions::acknowledgement acknowledgement(medium.local()); // Handler for acknowledgement packets
 * medium[dhyara::packets::type::acknowledgement] = acknowledgement;
 * 
 * // associate other actions ...
 * 
 * medium.start();
 * \endcode
 * \ingroup interface
 */
struct link{
    typedef std::function<void (const dhyara::peer_address&, const dhyara::frame&)> callback_type;
    typedef std::map<dhyara::packets::type, callback_type> handlers_map_type;
    typedef std::map<dhyara::packets::type, std::pair<std::size_t, std::size_t>> counters_map_type;
    typedef dhyara::network_fifo<dhyara::queue_size> fifo_type;
    typedef dhyara::xqueue<dhyara::message, dhyara::queue_size> xqueue_type;
    
    struct proxy{
        link& _link;
        dhyara::packets::type _type;
        
        inline proxy(link& l, dhyara::packets::type t): _link(l), _type(t){}
        template <typename ActionT>
        link& operator=(ActionT& action){
            _link.install(_type, std::ref(action));
            return _link;
        }
    };
    
    /**
     * Construct
     */
    link();
    
    /**
     * Initialize the medium
     */
    void init();
    
    /**
     * Sends a packet to an immediate neighbour directly in one hop.
     * 
     * \param addr target peer address 
     * \param type packet type
     * \param packet packet object
     */
    template <typename PacketT>
    bool send_local(const dhyara::peer_address& addr, packets::type type, const PacketT& packet){
        dhyara::utils::printer printer;
        printer.out(addr, packet);
        dhyara::frame frame(type, packet.size());
        dhyara::write(packet, frame._buffer);
        return transmit(addr, frame);
    }
    
    /**
     * Sends a packet via the best intermediate node. If the destination address is immediate neighbour then sends directly.
     * 
     * \param addr target peer address 
     * \param type packet type
     * \param packet packet object
     */
    template <typename PacketT>
    bool send(const dhyara::peer_address& addr, packets::type type, const PacketT& packet){
        dhyara::peer::address immediate = _routes.next(addr).via();
        return send_local(immediate.is_null() ? addr : immediate, type, packet);
    }
       
    /**
     * ESP Send Callback
     * 
     * \warning Only the ESP NOW sent callback should call this function
     * 
     * \param target target address
     * \param status status
     */
    void _esp_sent_cb(const uint8_t* target, esp_now_send_status_t status);
    
    /**
     * Process raw data received. Create a frame from the received data and passes that to queue for further procesing. Leaves wifi stack
     * 
     * \warning Only the ESP NOW receive callback should call this function
     * 
     * \param source source address
     * \param data raw data received
     * \param len length of the raw data
     */
    void _esp_rcvd_cb(const uint8_t* source, const uint8_t* data, int len);
    /**
     * Updates RSSI by using the frame headers received in the promiscous mode.
     * 
     * \warning Only the ESP Wifi promiscous rx callback should call this function
     * 
     * \param buffer received data
     * \param type promiscous packet type
     */
    void _esp_promiscous_rx_cb(void* buffer, wifi_promiscuous_pkt_type_t type);
    
    /**
     * Associate an action with a packet type. The operator()() overload of that action will be called with the received packet.
     * 
     * \param type packet type
     * \param cb The action
     */
    void install(dhyara::packets::type type, callback_type cb);
    
    /**
     * Conveniance function to install an action (associate an action with a packet type).
     * \code
     * dhyara::link medium;
     * dhyara::actions::beacon beacon(medium);
     * medium[dhyara::packets::type::beacon] = beacon;
     * \endcode
     * 
     * \param type packet type
     */
    proxy operator[](dhyara::packets::type type){
        return proxy(*this, type);
    }
    
    /**
     * start receive loop never returns
     */
    void start_rcv(std::size_t ticks = 0xffffffffUL);
    
#if DHYARA_ENABLED_SEND_QUEUEING
    /**
     * start receive loop never returns
     */
    void start_snd(std::size_t ticks = 0xffffffffUL);
#endif
    
    /**
     * immediate neighbourhood
     */
    inline dhyara::neighbourhood& neighbours() { return _neighbours; }
    /**
     * routing table
     */
    inline dhyara::routing& routes(){ return _routes; }
    
    /**
     * local address
     */
    inline const dhyara::peer::address& address() const { return _mac; }
    
    /**
     * reset counter
     */
    void reset(dhyara::packets::type type);
    /**
     * number of packets received for the given type
     * 
     * \param type packet type
     */
    std::size_t tx(dhyara::packets::type type) const;
    /**
     * number of packets recieved for the given type
     * 
     * \param type packet type
     */
    std::size_t rx(dhyara::packets::type type) const;
    
    /**
     * Maximum rssi in the neighbourhood
     */
    std::int8_t max_rssi() const;
    /**
     * Duration for which the node has not heard anything from its neighbourhood (in us).
     */
    dhyara::delay_type lost() const;
    
    /**
     * Associate with the target AP using the station interface
     */
    esp_err_t connect(const dhyara::peer_address& target);
    
    private:
        
        /**
         * Process a received frame
         * 
         * \warning should not be called directly. Supposed to be used as a callback.
         * 
         * \param address immediate source address
         * \param frame frame
         */
        void q_receive(const dhyara::peer_address& address, const dhyara::frame& frame);
        /**
         * Send raw data to an immediate destination. Called directly if transmission queueing is disabled. Otherwise called through start_snd
         * 
         * \param dest destination
         * \param data pointer to raw data
         * \param len lenth of the data
         */
        bool _transmit(const std::uint8_t* dest, const std::uint8_t* data, std::size_t len);
        /**
         * Depending of the configuration of transmission queueing, Transmits/Enequeues a frame for sending to the given destination address in <b>one hop</b>.
         * 
         * \param address destination address
         * \param frame frame
         */
        bool transmit(const dhyara::peer_address& address, const dhyara::frame& frame);
    
    private:
        dhyara::frame           _frame_rcv;     ///< Temporary buffer to hold the received frame which is then enqueued to _fifo
        fifo_type               _fifo_rcv;      ///< Queue to manage the received frames
        xqueue_type             _queue_snd;     ///< Queue to manage the frames to be sent
        dhyara::notifier        _notifier;      ///< A single element queue to wake up sending task
        dhyara::neighbourhood   _neighbours;
        handlers_map_type       _handlers;
        dhyara::routing         _routes;
        dhyara::peer::address   _mac;
        counters_map_type       _counters;
};

}

#endif // DHYARA_LINK_H
