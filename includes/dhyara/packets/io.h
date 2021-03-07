/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_PACKETS_IO_H
#define DHYARA_PACKETS_IO_H

#include "dhyara/packets/serialization.h"

namespace dhyara{
    
template <typename PacketT, typename OutIt>
OutIt write(const PacketT& packet, OutIt output){
    return serialization<PacketT>::write(packet, output);
}

template <typename PacketT, typename InIt>
InIt read(PacketT& packet, InIt input, std::size_t length = 0){
    return serialization<PacketT>::read(packet, input, length);
}
    
}

#endif // DHYARA_PACKETS_IO_H
