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
#include "esp_log.h"

namespace dhyara{
namespace utils{
namespace services{

struct ping{
    constexpr static const char* name() { return "ping"; }
    
    inline ping(): _batch(20), _wait(2), _help(false) {}
    // template<typename It>
    // inline clipp::parsing_result parse(It first, It last) {
    //     auto options = (
    //         clipp::value("target address", _target),
    //         clipp::option("-b", "--batch") & clipp::value("batch size", _batch),
    //         clipp::option("-w", "--wait")  & clipp::value("wait seconds", _wait),
    //         clipp::option("-h", "--help").set(_help, false).doc("show this help message")
    //     );
    //     return clipp::parse(first, last, options);
    // }
    template <typename It, typename StreamT>
    inline const char* run(It first, It last, StreamT& stream){
        auto command_options = (
            clipp::value("address", _target) % "target address",
            clipp::option("-b", "--batch") & clipp::value("size", _batch)   % "number of requests in one batch",
            clipp::option("-w", "--wait")  & clipp::value("seconds", _wait) % "number of seconds to wait after each batch of requests completes"
        );
        auto options = (
            command_options | clipp::option("-h", "--help").set(_help) % "show this help message"
        );
        auto res = clipp::parse(first, last, options);
        if(res.any_error() || _help){
            clipp::man_page man = clipp::make_man_page(options, name());
            stream << man; 
            return _help ? HTTPD_200 : HTTPD_400;
        }

        stream << name() << " " << _target << " -b" << _batch << " -w" << _wait << std::endl;
        stream << "Hello";
        return HTTPD_200;
    }
    private:
        std::string  _target;
        std::uint32_t _batch;
        std::uint32_t _wait;
        bool         _help;
};

}
}
}

#endif // DHYARA_SERVICES_PING_H