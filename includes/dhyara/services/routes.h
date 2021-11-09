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
#include <dhyara/cmd/cmd.hpp>
#include "esp_err.h"
#include <dhyara/services/stream.h>

namespace dhyara{
namespace services{

struct routes{
    constexpr static const char*    name         = "routes";
    constexpr static std::uint32_t  stack_size   = 4096;
    constexpr static std::uint8_t   priority     = 10;
    
    explicit inline routes(bool): _hops(0) {}
    cmd::args<std::uint8_t> options();
    esp_err_t run(services::stream& stream);
    private:
        std::uint8_t _hops;
};

}
}

#endif // DHYARA_SERVICES_ROUTES_H