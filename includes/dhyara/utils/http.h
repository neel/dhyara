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
#include "dhyara/link.h"

namespace dhyara{

namespace utils{
    
/**
 * HTTP utility to monitor the routing table
 * \ingroup utils
 */
struct http{
    
    /**
     * The HTTP Management console requires a reference to the link in order to access the routes and the neighbourhood.
     * \param link communication link
     */
    http(dhyara::link& link);
    /**
     * Start the HTTP Server
     */
    esp_err_t start();
    
    private:
        dhyara::link&  _link;
        httpd_config_t _config;
        httpd_handle_t _server;
        httpd_uri_t    _routes;
    private:
        static esp_err_t routes_handler(httpd_req_t* req);
        static esp_err_t info_handler(httpd_req_t* req);
    private:
        esp_err_t routes(httpd_req_t* req);
        esp_err_t info(httpd_req_t* req);
};

}

}

#endif // DHYARA_HTTP_H
