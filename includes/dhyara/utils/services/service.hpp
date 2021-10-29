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
#include "esp_err.h"
#include "esp_log.h"
#include <dhyara/utils/services/stream.h>

namespace dhyara{
namespace utils{
namespace services{

template <typename ServiceT>
struct service: private ServiceT{
    explicit service(httpd_req_t* req): _stream(req), _help(false){}

    template<class InputIterator>
    esp_err_t operator()(InputIterator first, InputIterator last){
        auto options = ServiceT::options() | clipp::option("-h", "--help").set(_help) % "show this help message";
        if(!clipp::parse(first, last, options)){
            ESP_LOGI("dhyara-services", "Error parsing arguments for service `%s`", ServiceT::name());
            _stream << clipp::make_man_page(options, ServiceT::name());
            _stream.finish(HTTPD_400);
            return ESP_OK;
        }else if(_help){
            _stream << clipp::make_man_page(options, ServiceT::name());
            _stream.finish(HTTPD_200);
            return ESP_OK;
        }else{
            ESP_LOGI("dhyara-services", "Running service `%s`", ServiceT::name());
            return ServiceT::run(_stream);
        }
    }

    private:
        services::stream _stream;
        bool             _help;
};

}
}
}

#endif // DHYARA_SERVICES_SERVICE_H