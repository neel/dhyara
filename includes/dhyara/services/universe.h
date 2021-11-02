/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_UNIVERSE_H
#define DHYARA_SERVICES_UNIVERSE_H

#include <cstdint>
#include <clipp/clipp.h>
#include "esp_err.h"
#include <dhyara/services/stream.h>

namespace dhyara{
namespace services{

struct universe{
    constexpr static const char*    name         = "universe";
    constexpr static std::uint32_t  stack_size   = 4096;
    constexpr static std::uint8_t   priority     = 10;
    
    explicit inline universe(bool): _only_neighbours(false), _only_peers(false) {}
    inline clipp::group options() {
        return clipp::joinable(
            clipp::option("-n").set(_only_neighbours, true) % "show neighbours",
            clipp::option("-u").set(_only_peers, true) % "show peers"
        );
    }
    esp_err_t run(services::stream& stream);
    private:
        bool _only_neighbours, _only_peers;
};

}
}

#endif // DHYARA_SERVICES_UNIVERSE_H