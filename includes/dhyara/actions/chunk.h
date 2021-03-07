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
 */
struct chunk: dhyara::actions::action<chunk, dhyara::packets::chunk>{
    typedef std::pair<dhyara::peer_address, std::uint8_t> incoming_data_source;
    typedef std::map<std::uint8_t, dhyara::packets::chunk, std::greater<std::uint8_t>> chunk_storage_type;
    typedef std::map<incoming_data_source, chunk_storage_type> incomming_data_map;
    typedef std::function<void (const dhyara::peer::address&, const dhyara::packets::data&)> data_callback_type;
    
    inline explicit chunk(dhyara::link& link): _link(link){}
    
    using action::operator();
    void operator()(const dhyara::peer_address& addr, const dhyara::packets::chunk& chunk);
    
    /**
     * set a callback which will be called once a complete data is received
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
