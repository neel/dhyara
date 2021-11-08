/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_PING_H
#define DHYARA_SERVICES_PING_H

#include <string>
#include <tuple>
#include <cstdlib>
#include <dhyara/address.h>
#include <clipp/clipp.h>
#include <dhyara/cmd/cmd.hpp>
#include "esp_err.h"
#include <dhyara/services/stream.h>
#include <dhyara/services/ping_impl.h>

namespace dhyara{
namespace services{

struct ping{
    constexpr static const char*    name         = "ping";
    constexpr static std::uint32_t  stack_size   = 3072;
    constexpr static std::uint8_t   priority     = 19;
    
    explicit inline ping(bool low_io): _low_io(low_io), _batch(20), _count(1), _wait(2) {}
    clipp::group options();
    void options2() {
        auto chain = (
            cmd::pos("address", _target) % "target address",
            cmd::arg("-b", "--batch") & cmd::value(_batch, "size")     % "number of ICMPQ requests in one batch",
            cmd::arg("-c", "--count") & cmd::value(_count, "count")    % "number of batches to send",
            cmd::arg("-w", "--wait")  & cmd::value(_wait,  "seconds")  % "number of seconds to wait after each batch of requests, for receiving all responses"
        );
    }
    esp_err_t run(services::stream& stream);
    
    private:
        bool         _low_io;
        std::string  _target;
        std::uint32_t _batch;
        std::uint32_t _count;
        std::uint32_t _wait;
};

}
}

#endif // DHYARA_SERVICES_PING_H