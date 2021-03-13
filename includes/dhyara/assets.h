/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_ASSETS_H
#define DHYARA_ASSETS_H

#include <cstdint>

namespace dhyara{
    
namespace assets{
    
extern const std::uint8_t routing_css_start[] asm("_binary_routing_css_start");
extern const std::uint8_t routing_css_end[]   asm("_binary_routing_css_end");
    
}
    
}

#endif // DHYARA_ASSETS_H
