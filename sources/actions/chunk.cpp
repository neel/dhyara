/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
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
