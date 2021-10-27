/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_H
#define DHYARA_SERVICES_H

#include <cstdint>
#include <dhyara/address.h>

namespace dhyara{
namespace services{

struct ping;

struct ping_service{
    friend struct services::ping;

    private:
        explicit ping_service(const dhyara::address& target, std::uint32_t batch);
    private:
        dhyara::address _target;
        std::uint32_t   _batch;
        std::uint32_t   _count;
        std::uint32_t   _sleep;
        std::uint8_t    _wait;
};

struct ping{
    using service = ping_service;
};

}
}

#endif // DHYARA_SERVICES_H