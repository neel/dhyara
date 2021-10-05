/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_ACTION_H
#define DHYARA_ACTIONS_ACTION_H

#include "dhyara/frame.h"
#include "dhyara/peer.h"
#include <functional>
#include <map>
#include "dhyara/utils/printer.h"

namespace dhyara{
    
namespace actions{
    
/**
 * All actions must subclass from this template. These actions can then be installed to the link.
 * \ingroup actions
 * \code
 * // Assuming X is a type of packet, identified by type dhyara::packets::type::x and defined in class dhyara::packets::X
 * struct X: dhyara::action<X, dhyara::packets::X>{
 *      using action::operator(); // expose the base class's operator() overload
 *      void operator()(const dhyara::address& addr, const dhyara::packets::X& packet); // provide an operator() overload to consume the received packet 
 * };
 * \endcode
 */
template <typename DerivedT, typename PacketT>
struct action{
    typedef std::function<void (const dhyara::address&, const PacketT&)> slot_type;
    
    /**
     * Default action constructor
     */
    action(): _last(0) {}
    /**
     * An action cannot be copy-constructed
     */
    action(const action<DerivedT, PacketT>&) = delete;
    /**
     * An action is not copy-assignable
     */
    action<DerivedT, PacketT>& operator=(const action<DerivedT, PacketT>&) = delete;
    /**
     * Creates a packet from the received frame. Passes that packet to the Derived action's operator() overload. Calls the slots connected.
     */
    void operator()(const dhyara::address& address, const dhyara::frame& frame){
        static PacketT packet;
        dhyara::read(packet, frame._buffer, frame.length());
//         dhyara::utils::printer printer;
//         printer.in(address, packet);
        static_cast<DerivedT*>(this)->operator()(address, packet);
        for(auto& pair: _slots){
            if(pair.second){
                pair.second(address, packet);
            }
        }
    }
    
    /**
     * Connect a slot (callback) which will be called after the action is executed.
     * 
     * \param slot The callback to connect
     * \return the slot id
     */
    std::size_t connect(slot_type slot){
        std::size_t id = ++_last;
        _slots.insert(std::make_pair(id, slot));
        return id;
    }
    
    /**
     * Disconnects an already connected slot by id
     * 
     * \param id slot id returned by the connect call
     */
    void disconnect(std::size_t id){
        auto it = _slots.find(id);
        if(it != _slots.end()){
            _slots.erase(it);
        }
    }
    
    private:
        std::map<std::size_t, slot_type> _slots;
        std::size_t _last;
};
    
}

}

#endif // DHYARA_ACTION_H
