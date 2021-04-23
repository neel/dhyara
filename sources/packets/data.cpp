/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "dhyara/packets/data.h"
#include <algorithm>

std::uint8_t dhyara::packets::data::counter = 0;

dhyara::packets::chunk dhyara::packets::data::prepare(std::uint8_t n) const{
    auto shift  = std::min(static_cast<std::size_t>(n * packets::chunk::capacity), _buffer.length());
    auto begin  = _buffer.begin() + shift;
    auto length = std::min(static_cast<std::size_t>(packets::chunk::capacity), (_buffer.length() - shift));
    
    return dhyara::packets::chunk(_target, _source, begin, length, _packet, (chunks() - n - 1));
}

void dhyara::packets::data::add(const dhyara::packets::chunk& chunk){
    fill(chunk.begin(), chunk.end());
}

void dhyara::packets::data::reserve(std::uint32_t size){
    _buffer.reserve(size);
}
