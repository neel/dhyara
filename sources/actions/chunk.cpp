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

#include "dhyara/actions/chunk.h"
#include "dhyara/link.h"

void dhyara::actions::chunk::operator()(const dhyara::address& addr, const dhyara::packets::chunk& chunk){
    dhyara::address target = chunk.target(), source = chunk.source();
    std::uint8_t packet = chunk.packet(), pending = chunk.pending();
    if(target != _link.address()){
        _link.send(target, dhyara::packets::type::chunk, chunk);
    } else {
        auto it = _buffer.find(incoming_data_source(source, packet));
        if(it == _buffer.end()){
            if(!chunk.is_last()){
                chunk_storage_type initial_storage;
                initial_storage.insert(std::make_pair(pending, chunk));
                _buffer.insert(std::make_pair(incoming_data_source(source, packet), initial_storage));
            }else{
                dhyara::packets::data data(target, source, packet);
                data.add(chunk);
                if(_slot){
                    _slot(addr, data);
                }
            }
        }else{
            chunk_storage_type& storage = it->second;
            storage.insert(std::make_pair(pending, chunk));
            if(chunk.is_last() || 0 == storage.rbegin()->first){
                dhyara::packets::data data(target, source, packet);
                data.reserve(storage.size() * 233);
                std::uint8_t last = storage.begin()->first;
                bool completed = true;
                for(const auto& kv: storage){
                    if(last - kv.first > 1){
                        completed = false;
                    }
                    data.add(kv.second);
                    last = kv.first;
                }
                if(completed){
                    if(_slot){
                        _slot(addr, data);
                    }
                    _buffer.erase(it);
                }
            }
        }
    }
}
