/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_PHY_RATE_H
#define DHYARA_SERVICES_PHY_RATE_H

#include <cstdint>
#include <dhyara/cmd/cmd.hpp>
#include "esp_err.h"
#include <dhyara/services/stream.h>

namespace dhyara{
namespace services{

struct phy_rate{
    constexpr static const char*    name         = "phy-rate";
    constexpr static std::uint32_t  stack_size   = 4096;
    constexpr static std::uint8_t   priority     = 10;
    
    inline phy_rate(): _disable(false) {}
    dhyara::cmd::args<bool, bool, std::string> options();
    esp_err_t run(services::stream& stream);

    private:
        std::string _phy_rate;
        bool _disable;
        bool _list;
};

}
}

#endif // DHYARA_SERVICES_PHY_RATE_H