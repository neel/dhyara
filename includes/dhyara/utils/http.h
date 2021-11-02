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
#include "dhyara/services/registry.hpp"
#include "dhyara/services/service.hpp"
#include "dhyara/services/ping.h"

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
    private:
        httpd_uri_t    _index_html;
        httpd_uri_t    _routes_html;
        httpd_uri_t    _peers_html;
        httpd_uri_t    _style;
        httpd_uri_t    _icons;
        httpd_uri_t    _info;
        httpd_uri_t    _counter;
        httpd_uri_t    _routes;
        httpd_uri_t    _peers;
        httpd_uri_t    _command;
    private:
        static esp_err_t index_html_handler(httpd_req_t* req);
        static esp_err_t routes_html_handler(httpd_req_t* req);
        static esp_err_t peers_html_handler(httpd_req_t* req);
        static esp_err_t style_handler(httpd_req_t* req);
        static esp_err_t icons_handler(httpd_req_t* req);
        static esp_err_t info_handler(httpd_req_t* req);
        static esp_err_t counter_handler(httpd_req_t* req);
        static esp_err_t routes_handler(httpd_req_t* req);
        static esp_err_t peers_handler(httpd_req_t* req);
        static esp_err_t command_handler(httpd_req_t* req);
    private:
        esp_err_t index_html(httpd_req_t* req);
        esp_err_t routes_html(httpd_req_t* req);
        esp_err_t peers_html(httpd_req_t* req);
        esp_err_t style(httpd_req_t* req);
        esp_err_t icons(httpd_req_t* req);
        esp_err_t info(httpd_req_t* req);
        esp_err_t counter(httpd_req_t* req);
        esp_err_t routes(httpd_req_t* req);
        esp_err_t peers(httpd_req_t* req);
        esp_err_t command(httpd_req_t* req);
    private:
        using registry_type =  services::registry<services::ping>;

        registry_type       _registry;
};

}

}

#endif // DHYARA_HTTP_H
