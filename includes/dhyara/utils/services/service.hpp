/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_SERVICES_SERVICE_H
#define DHYARA_SERVICES_SERVICE_H

#include <clipp/clipp.h>
#include <esp_http_server.h>

namespace dhyara{
namespace utils{
namespace services{

template <typename ServiceT, typename StreamT>
struct service: private ServiceT{
    explicit service(StreamT& stream): _stream(stream){}

    template<class InputIterator>
    const char* operator()(InputIterator first, InputIterator last){
        auto options = ServiceT::options();
        if(!clipp::parse(first, last, options)){
            clipp::man_page man = clipp::make_man_page(options, ServiceT::name());
            _stream << man; 
            return HTTPD_400;
        }else{
            return ServiceT::run();
        }
    }

    private:
        StreamT& _stream;
};

}
}
}

#endif // DHYARA_SERVICES_SERVICE_H