/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_ROUTES_H
#define DHYARA_SERVICES_ROUTES_H

#include <cstdint>
#include <clipp/clipp.h>
#include "esp_err.h"
#include <dhyara/services/stream.h>

namespace dhyara{
namespace services{

struct routes{
    constexpr static const char*    name         = "routes";
    constexpr static std::uint32_t  stack_size   = 4096;
    constexpr static std::uint8_t   priority     = 10;
    
    explicit inline routes(bool): _hops(0), _only_routes(false), _only_best(false) {}
    inline clipp::group options() {
        return (
            clipp::option("-h", "--hops") & clipp::value("N", _hops) % "Only show the routes that are N or more hops away",
            (
                clipp::option("-r").set(_only_routes, false) % "Show routing table only" |
                clipp::option("-b").set(_only_best, false) % "Show best routes only"
            )
        );
    }
    esp_err_t run(services::stream& stream);
    private:
        std::uint8_t _hops;
        bool _only_routes, _only_best;
};

}
}

#endif // DHYARA_SERVICES_ROUTES_H