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
#include <cstdlib>
#include <esp_http_server.h>
#include <dhyara/address.h>
#include <clipp/clipp.h>

namespace dhyara{
namespace utils{
namespace services{

struct ping{
    constexpr static const char* name() { return "ping"; }
    
    inline ping(): _batch(20), _wait(2) {}
    inline clipp::group options() {
        return (
            clipp::value("target address", _target),
            clipp::option("-b", "--batch").set(_batch).doc("number of requests in one batch"),
            clipp::option("-w", "--wait").set(_wait).doc("number of seconds to wait for receiving all responses")
        );
    }
    inline const char* run(){
        // expected that the args has already been parsed
        return HTTPD_200;
    }
    private:
        std::string  _target;
        std::uint8_t _batch;
        std::uint8_t _wait;
};

}
}
}

#endif // DHYARA_SERVICES_PING_H