/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_IDENTIFY_H
#define DHYARA_SERVICES_IDENTIFY_H

#include <cstdint>
#include <clipp/clipp.h>
#include "esp_err.h"
#include <dhyara/services/stream.h>

namespace dhyara{
namespace services{

struct identify{
    constexpr static const char*    name         = "identify";
    constexpr static std::uint32_t  stack_size   = 4096;
    constexpr static std::uint8_t   priority     = 10;
    
    explicit inline identify(bool) {}
    inline clipp::group options() {
        return clipp::group();
    }
    esp_err_t run(services::stream& stream);
};

}
}

#endif // DHYARA_SERVICES_IDENTIFY_H