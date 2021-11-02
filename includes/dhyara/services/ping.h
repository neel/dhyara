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
#include <dhyara/services/stream.h>
#include <dhyara/services/ping_impl.h>

namespace dhyara{
namespace services{

struct ping{
    constexpr static const char*    name         = "ping";
    constexpr static std::uint32_t  stack_size   = 3072;
    constexpr static std::uint8_t   priority     = 19;
    
    inline ping(bool low_io): _low_io(low_io), _batch(20), _count(1), _wait(2) {}
    inline clipp::group options() {
        return (
            clipp::value("address", _target) % "target address",
            clipp::option("-b", "--batch") & clipp::value("size",    _batch) % "number of ICMPQ requests in one batch",
            clipp::option("-c", "--count") & clipp::value("count",   _count) % "number of batches to send",
            clipp::option("-w", "--wait")  & clipp::value("seconds", _wait)  % "number of seconds to wait after each batch of requests, for receiving all responses"
        );
    }
    esp_err_t run(services::stream& stream){
        dhyara::address target(_target);
        if(target.valid()){
            services::ping_impl impl(stream, _count, _batch, _wait);
            impl.low_io(_low_io);
            impl(target);
            stream.finish();
        }
        return ESP_OK;
    }
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