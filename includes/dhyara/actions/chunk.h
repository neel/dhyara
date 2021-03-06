/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ACTIONS_CHUNK_H
#define DHYARA_ACTIONS_CHUNK_H

#include <map>
#include <cstdint>
#include <functional>
#include "dhyara/actions/action.h"
#include "dhyara/packets/chunk.h"

namespace dhyara{
    
struct link;
    
namespace actions{
    
/**
 * action to handle an incoming chunk packet
 * \ingroup actions
 */
struct chunk: dhyara::actions::action<chunk, dhyara::packets::chunk>{
    typedef std::pair<dhyara::address, std::uint8_t> incoming_data_source;
    typedef std::map<std::uint8_t, dhyara::packets::chunk, std::greater<std::uint8_t>> chunk_storage_type;
    typedef std::map<incoming_data_source, chunk_storage_type> incomming_data_map;
    typedef std::function<void (const dhyara::address&, const dhyara::packets::data&)> data_callback_type;
    
    /**
     * A chunk action needs to have access to the link in order to relay the chunk if it is an intermediate node.
     * 
     * \param link a reference to the link
     */
    inline explicit chunk(dhyara::link& link): _link(link){}
    
    using action::operator();
    /**
     * The received chunk is processed in this function.
     * 
     * \param addr source address 
     * \param chunk the received chunk 
     */
    void operator()(const dhyara::address& addr, const dhyara::packets::chunk& chunk);
    
    /**
     * set a callback which will be called once a complete data is received
     * \param slot the callback
     */
    inline void on_data(data_callback_type slot) { _slot = slot; }
    
    private:
        dhyara::link& _link;
        incomming_data_map _buffer;
        data_callback_type _slot;
};
    
}

}

#endif // DHYARA_ACTIONS_CHUNK_H
