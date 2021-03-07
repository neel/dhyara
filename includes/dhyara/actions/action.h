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
 * \code
 * // Assuming X is a type of packet, identified by type dhyara::packets::type::x and defined in class dhyara::packets::X
 * struct X: dhyara::action<X, dhyara::packets::X>{
 *      using action::operator(); // expose the base class's operator() overload
 *      void operator()(const dhyara::peer_address& addr, const dhyara::packets::X& packet); // provide an operator() overload to consume the received packet 
 * };
 * \endcode
 */
template <typename DerivedT, typename PacketT>
struct action{
    typedef std::function<void (const dhyara::peer_address&, const PacketT&)> slot_type;
    
    action(): _last(0) {}
    action(const action<DerivedT, PacketT>&) = delete;
    action<DerivedT, PacketT>& operator=(const action<DerivedT, PacketT>&) = delete;
    /**
     * Creates a packet from the received frame. Passes that packet to the Derived action's operator() overload. Calls the slots connected.
     */
    void operator()(const dhyara::peer_address& address, const dhyara::frame& frame){
        PacketT packet;
        dhyara::read(packet, frame._buffer, frame.length());
        dhyara::utils::printer printer;
        printer.in(address, packet);
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
