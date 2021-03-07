/*
 * Copyright (c) 2021, Neel Basu <neel.basu.z@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY Neel Basu <neel.basu.z@gmail.com> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Neel Basu <neel.basu.z@gmail.com> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
 * Link Layer Medium
 * \code
 * dhyara::link medium; // A communication medium 
 * 
 * dhyara::actions::beacon beacon(medium.local()); // Handler for beacon packets
 * medium.install(dhyara::packets::type::beacon, std::ref(beacon));
 * 
 * dhyara::actions::acknowledgement acknowledgement(medium.local()); // Handler for acknowledgement packets
 * medium[dhyara::packets::type::acknowledgement] = acknowledgement;
 * 
 * // associate other actions ...
 * 
 * medium.start();
 * \endcode
 */
struct link{
    typedef std::function<void (const dhyara::peer_address&, const dhyara::frame&)> callback_type;
    typedef std::map<dhyara::packets::type, callback_type> handlers_map_type;
    typedef std::map<dhyara::packets::type, std::pair<std::size_t, std::size_t>> counters_map_type;
    typedef dhyara::network_fifo<dhyara::queue_size> fifo_type;
    
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
     * \param wait If wait is true then enqueue to send instead of sending immediately
     */
    template <typename PacketT>
    bool send_local(const dhyara::peer_address& addr, packets::type type, const PacketT& packet, std::size_t ticks = 0){
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
     * \param wait If wait is true then enqueue to send instead of sending immediately
     */
    template <typename PacketT>
    bool send(const dhyara::peer_address& addr, packets::type type, const PacketT& packet, std::size_t ticks = 0){
        dhyara::peer::address immediate = _routes.next(addr).via();
        return send_local(immediate.is_null() ? addr : immediate, type, packet, ticks);
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
    
    private:
        /**
         * Process a outgoing frame which is to be delivered to the given destination address in ONE HOP
         * 
         * \warning should not be called directly. Supposed to be used as a callback to the queue.
         * 
         * \param address destination address
         * \param frame frame
         */
        bool q_send(const dhyara::peer_address& address, const dhyara::frame& frame);
        
        /**
         * Process a received frame
         * 
         * \warning should not be called directly. Supposed to be used as a callback.
         * 
         * \param type packet type
         * \param address immediate source address
         * \param frame frame
         */
        void q_receive(const dhyara::peer_address& address, const dhyara::frame& frame);
        /**
         * send raw data to an immediate destination
         * 
         * \param dest destination
         * \param data pointer to raw data
         * \param len lenth of the data
         */
        bool transmit(const std::uint8_t* dest, const std::uint8_t* data, std::size_t len);
        /**
         * send a frame to the given destination address in ONE HOP
         * 
         * \param address destination address
         * \param frame frame
         */
        bool transmit(const dhyara::peer_address& address, const dhyara::frame& frame);
    
    private:
        dhyara::frame           _rcv_frame;
        fifo_type               _fifo;
        dhyara::neighbourhood   _neighbours;
        handlers_map_type       _handlers;
        dhyara::routing         _routes;
        dhyara::peer::address   _mac;
        counters_map_type       _counters;
};

}

#endif // DHYARA_LINK_H
