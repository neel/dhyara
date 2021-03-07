/*
 * Copyright (c) 2020, Sunanda Bose (a.k.a. Neel Basu)
 * All rights reserved.
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DHYARA_HTTP_H
#define DHYARA_HTTP_H

#include <esp_http_server.h>
#include "dhyara/routing.h"

namespace dhyara{

namespace utils{
    
/**
 * @todo write docs
 */
struct http{
    
    http(dhyara::routing& routes);
    
    private:
        dhyara::routing& _routing;
        httpd_config_t _config;
        httpd_handle_t _server;
        httpd_uri_t    _routes;
    private:
        static esp_err_t routes_handler(httpd_req_t* req);
    private:
        esp_err_t routes(httpd_req_t* req);
};

}

}

#endif // DHYARA_HTTP_H
