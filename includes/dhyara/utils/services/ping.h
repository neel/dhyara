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
#include <esp_http_server.h>
#include <dhyara/address.h>
#include <clipp/clipp.h>
#include "esp_err.h"
#include "esp_log.h"
#include <vector>
#include <dhyara/defs.h>
#include "dhyara/packets/echo_reply.h"
#include <dhyara/utils/services/stream.h>
#include <dhyara/utils/services/ping_impl.h>

namespace dhyara{
namespace utils{
namespace services{

struct ping{
    constexpr static const char* name() { return "ping"; }
    
    inline ping(): _batch(20), _wait(2) {}
    inline clipp::group options() {
        return (
            clipp::value("address", _target) % "target address",
            clipp::option("-b", "--batch") & clipp::value("size", _batch)   % "number of ICMPQ requests in one batch",
            clipp::option("-c", "--count") & clipp::value("count", _batch)  % "number of batches to send",
            clipp::option("-w", "--wait")  & clipp::value("seconds", _wait) % "number of seconds to wait after each batch of requests, for receiving all responses"
        );
    }
    esp_err_t run(services::stream& stream){
        services::ping_impl impl(stream, 1, _batch, _wait);
        impl(_target);
        stream.finish();
        return ESP_OK;
    }
    private:
        std::string  _target;
        std::uint32_t _batch;
        std::uint32_t _wait;
};

}
}
}

#endif // DHYARA_SERVICES_PING_H